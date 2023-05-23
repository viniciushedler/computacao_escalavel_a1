'''
    This file contains the classes that simulate the road and the cars.

    The Road class contains the road itself, and the cars on it.

    The Car class contains the car itself, and the logic to decide what to do.

    The Collision class contains the cars that collided, and the countdown until they disappear.
'''

import random
import string
import datetime
import os
from typing import Union
from ansi import ANSI
from parameters import *  #pylint:disable = wildcard-import, unused-wildcard-import

import highway_pb2_grpc
import highway_pb2
import grpc
import sys

def send_message(message_data):

    with grpc.insecure_channel('localhost:50051') as channel:
        
        stub = highway_pb2_grpc.HighwaySenderStub(channel)

        highway_request = highway_pb2.HighwayRequest(data = message_data)
        reply = stub.SendHighway(highway_request)

output_folder = "roads"
temp_folder = "temp"

class Output():
    message = ""

    def __init__(self) -> None:
        self.message = ""
    
    def write(self, text):
        self.message += text

def model_from_plate(plate):
    '''
    Returns the name of a car model given a plate
    '''

    # Turns the plate to an integer by summing the ascii value of each character
    n = sum([ ord(s) for s in plate ])

    # Gets an ordered list of the car model names
    keys = list(MODELS.keys())
    keys.sort()

    # Gets the desired index
    i = n%len(MODELS)

    # Returns the key at given index
    return keys[i]


class Collision():
    '''
        This class contains the cars that collided, and the countdown until they disappear.

        Attributes:
            collided_cars: A list containg the cars that collided.
            countdown: The countdown until the collision disappears.

        Methods:
            add: Adds a car to the collision list.
    '''

    def __init__(self, collided_cars: list['Car'], countdown: int):
        self.collided_cars = collided_cars
        self.countdown = countdown
        self.lane, self.length = self.pos = collided_cars[0].pos

    def add(self, car):
        '''
            Adds a car to the collision list.
        '''
        self.collided_cars.append(car)
        self.countdown += 1


class Car():
    '''
        This class contains the car itself, and the logic to decide what to do.

        Attributes:
            road: The road the car is on.
            risk: The risk of collision the car has.
            speed_min: The minimum speed the car can have.
            speed_max: The maximum speed the car can have.
            acceleration_max: The maximum acceleration the car can have.
            acceleration_min: The minimum acceleration the car can have.
            lange_change_prob: The probability of changing lanes arbitrarily

            _lane: The lane the car is on.
    '''

    def __init__(self, road: 'Road', lane: int, speed_min: int, speed_max:int, acceleration_min:int, acceleration_max:int, lane_change_prob, collsion_risk: float):
        # Road
        self.road = road

        # Car parameters
        #self.plate = secrets.token_urlsafe(4)
        self.plate = ''.join([random.choice(string.ascii_uppercase) for _ in range(3)]) + str(random.randint(0,9)) + random.choice(string.ascii_uppercase) + str(random.randint(0,9)) + str(random.randint(0,9))
        self.model = model_from_plate(self.plate)
        self.risk = collsion_risk
        self.speed_min = speed_min
        self.speed_max = speed_max
        self.acceleration_min = acceleration_min
        self.acceleration_max = acceleration_max
        self.lange_change_prob = lane_change_prob

        # Car variables
        self._lane, self._length = self._pos = (lane, 0)
        self.speed = self.speed_min
        self.collided = False


    @property
    def pos(self):
        '''
            Returns the position of the car.
        '''
        return self._pos

    @pos.setter
    def pos(self, new_value: tuple[int]):
        '''
            Sets the position attribute of the car and then
            update the car's position in it's road attribute.
            Also calls a lane and length update
        '''
        # Tells the road to move the car in it
        self.road.move(self._pos, new_value)

        # Saves the new position of the car
        self._pos = new_value
        # Saves in the other variables as well
        if self.lane != new_value[0]:
            self.lane = new_value[0]
        if self.length != new_value[1]:
            self.length = new_value[1]


    @property
    def lane(self):
        '''
            Returns the lane the car is on.
        '''
        return self._lane

    @lane.setter
    def lane(self, new_value):
        '''
            Updates the car's lane and calls a position update
        '''
        # Saves the new position of the car
        self._lane = new_value
        # Saves in self.pos as well
        if self.pos[0] != new_value:
            self.pos = new_value, self.length

    @property
    def length(self):
        '''
            Returns the length the car is on.
        '''
        return self._length

    @length.setter
    def length(self, new_value):
        '''
            Updates the car's length and calls a position update
        '''
        # Saves the new position of the car
        self._length = new_value
        # Saves in self.pos as well
        if self.pos[1] != new_value:
            self.pos = self.lane, new_value


    def accelerate(self):
        '''
            Accelerates the car.

            Chooses a random acceleration, and adds it to the speed.
            Corrects the speed if it's below the minimum or above the maximum.
        '''

        # Chooses how much to accelerate
        acc = random.randint(self.acceleration_min, self.acceleration_max)

        # Accelerates
        self.speed += acc

        # If speed is below min or above max, correct it
        if self.speed < self.speed_min:
            self.speed = self.speed_min
        elif self.speed > self.speed_max:
            self.speed = self.speed_max

    def change_lane(self, force = False):
        '''
            Decides whether to change lanes, considering 
            availability of the lane and whether the driver is being careful
        '''

        available_lanes = []

        is_forward = 0 < self.lane < self.road.lanes_f - 1

        if is_forward:
            # If can diminish lane
            if (self.lane > 0) and (force or self.road.is_empty(self.lane-1, self.length)):
                available_lanes.append(self.lane-1)
            # If can increase lane
            if self.lane < (self.road.lanes_f - 1) and (force or self.road.is_empty(
                self.lane + 1,
                self.length)
            ):
                available_lanes.append(self.lane+1)
        else:
            # If can diminish lane
            if (self.lane > self.road.lanes_f) and (force or self.road.is_empty(self.lane-1, self.length)):
                available_lanes.append(self.lane-1)
            # If can increase lane
            if self.lane < (self.road.lanes_total - 1) and (force or self.road.is_empty(
                self.lane + 1,
                self.length)
            ):
                available_lanes.append(self.lane+1)

        # Returns if no lane is available
        if len(available_lanes) == 0:
            return False

        # Moves to a new lane
        self.lane = random.choice(available_lanes)
        return True

    def decide_movement(self):
        '''
            Decides which movement to do.

            This function checks if there is something on the way.
            If there's nothing, it moves foward.
            And if there's something, it tries to change lanes, which may cause a collision.

            If it changes lanes and it's way is still obstructed it deaccelerates.

            Afterwards, moves forward
            
            At the end, independently, changes lane arbitrarily
        '''

        # 'free' variable, will decide whether to move straight or do something else
        free = True
        force = random.random() > 1 - self.risk

        # Check if the path is empty from it's current position to it's current_position + speed
        free, length_new = self.road.path_is_empty(
            self.lane,
            self.length +1,
            self.length + self.speed
        )

        # Nothing was on the way or wanted to collide: move forward
        if free or force:
            self.length = length_new
            return
        # The following code only executes if the car didn't move already

        # Tries to change lanes, and if it fails, deaccelerates
        if not self.change_lane():
            self.speed += self.acceleration_min

        # Now, with new lane/speed (or not), gets maximum length traveled
        _, length_new = self.road.path_is_empty(self.lane, self.length +1, self.length + self.speed)
        self.length = length_new

        # Arbitrary lane change
        if random.random() > 1 - self.lange_change_prob:
            self.change_lane(force)

class Road():
    '''
        Road class.

        Parameters:
            length: The length of the road.
            lanes: The number of lanes.
            road: The road itself, a list of lists of cars and collisions.
            collision_countdown: The number of cycles a collision lasts.
            car_spawn_prob: The probability of a car spawning in a lane in a cycle.
    '''
    def __init__(self, name, lanes_f, lanes_b, length, speed_limit, prob_of_new_car, prob_of_changing_lane, prob_of_collision, car_speed_min, car_speed_max, car_acc_min, car_acc_max, collision_fix_time):

        # Road parameters
        self.length = length
        self.lanes_f = lanes_f
        self.lanes_b = lanes_b
        self.lanes_total = lanes_f + lanes_b
        self.road: list[Union[Car, Collision, None]]=[[None]*self.length for _ in range(self.lanes_total)]
        self.collision_countdown = collision_fix_time
        self.car_spawn_prob = prob_of_new_car
        self.speed_limit = speed_limit
        self.name = name
        self.prob_of_changing_lane = prob_of_changing_lane
        self.prob_of_collision = prob_of_collision
        self.car_speed_min = car_speed_min
        self.car_speed_max = car_speed_max
        self.car_acc_min = car_acc_min
        self.car_acc_max = car_acc_max

        # Road variables
        self.collisions: list[Collision] = []
        self.cycle_total = 0
        self.collision_total = 0

        # Debug
        self.counter = 0

    def create_cars(self):
        '''
            Creates cars in the road.
        '''

        # For every lane, choose whether to spawn a car
        for lane in range(self.lanes_total):
            if random.random() > 1 - self.car_spawn_prob and self.road[lane][0] is None:
                self.road[lane][0] = Car(self, lane, self.car_speed_min, self.car_speed_max, self.car_acc_min, self.car_acc_max, self.prob_of_changing_lane, self.prob_of_collision)

    def is_empty(self, lane: int, length:int) -> bool:
        '''
            Returns whether a position is empty or not.
        '''
        if length >= self.length:
            return True
        return self.road[lane][length] is None

    def move(self, position_old, position_new):
        '''
            Inside self.road, copies the car at position_old
            to position_new and then removes it from position_old
            There are four possibilities depending on position_new:
                position_new is beyond the end of the road: removes the car and returns
                position_new is empty: moves the car normally
                position_new contains a car: creates a collision
                position_new contains a collision: adds the car to the collision

            At the end the car will be removed from it's old position
        '''

        # Create some variables for readability
        lane_o, length_o = position_old
        lane_n, length_n = position_new
        object_o = self.road[lane_o][length_o]

        if length_n >= self.length - 1:
            self.road[lane_o][length_o] = None
            return

        object_n = self.road[lane_n][length_n]
        # Check if new position is beyond the road

        # Check if new position is empty
        if self.is_empty(lane_n, length_n):
            # Creates a copy of the car in the new position
            self.road[lane_n][length_n] = object_o

        # Check if the position contains another car
        elif isinstance(object_n, Car):
            # Support variable
            # Note: it is important that the new position car comes first in the list
            # Because, at this moment, the old position car still has it's old position saved in it
            # The first car position is used when creating the 'Collision' object
            collided_cars = [
                object_n,
                object_o
            ]
            # Creates a collision object
            self.road[lane_n][length_n] = Collision(collided_cars, self.collision_countdown)
            self.collisions.append(self.road[lane_n][length_n])
            self.collision_total += 1

        # Check if the position contains a collision already
        elif isinstance(object_n, Collision):
            self.road[lane_n][length_n].add(object_o)
            self.collision_total += 1

        # If didn't fall into any condition before, raise an error
        else:
            raise ValueError(f"Value at ({lane_n}, {length_n}) is {type(object_n)}." +
                " Expected None, Car or Collision")

        # Removes the occurence of the car in the old position
        self.road[lane_o][length_o] = None

    def all_decide_movement(self):
        '''
            Calls the 'decide_movement' method of every car in the road.
            Note the order this is done: car farthest from the start first, ti-breaking by lanes (smallest first)
        '''
        for length in range(self.length, -1, -1):
            for lane in range(self.lanes_total):
                # In this code, any 'pseudo_car' instance is a cell of the road which type we don't know
                pseudo_car = self.road[lane][length]
                if isinstance(pseudo_car, Car):
                    pseudo_car.decide_movement()

    def remove_collisions(self):
        '''
            Removes collisions that have ended.
            Deprecated.
            The removal is now being done at self.cycle(), in order to use the self.road traversal
        '''
        for collision in self.collisions:
            collision.countdown -= 1
            if collision.countdown <= 0:
                if isinstance(self.road[collision.lane][collision.length], Collision):
                    self.road[collision.lane][collision.length] = None

    def cycle(self):
        '''
            Performs a cycle of the road.

            Calls remove_collisions once every cycle.
        '''
        #self.remove_collisions()

        for length in range(self.length - 1, -1, -1):
            for lane in range(self.lanes_total):
                pseudo_car = self.road[lane][length]
                #if type(pseudo_car) == Car:
                if isinstance(pseudo_car, Car):
                    pseudo_car.accelerate()
                    pseudo_car.decide_movement()
                elif isinstance(pseudo_car, Collision):
                    pseudo_car.countdown -= 1
                    if pseudo_car.countdown <= 0:
                        self.road[lane][length] = None

        self.create_cars()
        self.cycle_total += 1

    def __str__(self):
        '''
            Creates a visualization of 'self.road'
            In it:
                Nones are represented by '.' in order no to draw attention
                Cars are colored green and represented by their current speed
                Collisions are colored red and represented by their countdown
        '''
        string = ""

        # lanes forwards
        for lane in range(self.lanes_f):
            for pseudo_car in self.road[lane]:
                if pseudo_car is None:
                    string += "   "
                #elif type(pseudo_car) == Car:
                elif isinstance(pseudo_car, Car):
                    string += " " + ANSI.colored_str(str(pseudo_car.speed), 'green') + " "
                #elif type(pseudo_car) == Collision:
                elif isinstance(pseudo_car, Collision):
                    string += " " + ANSI.colored_str(str(pseudo_car.countdown), 'red') + " "
            string += "\n"

        for _ in range(self.length):
            string += "---"
        string += '\n'

        # lanes backwards
        for lane in range(self.lanes_f, self.lanes_total):
            for car_i in range(self.length-1, -1, -1):
                pseudo_car = self.road[lane][car_i]
                if pseudo_car is None:
                    string += "   "
                #elif type(pseudo_car) == Car:
                elif isinstance(pseudo_car, Car):
                    string += "-" + ANSI.colored_str(str(pseudo_car.speed), 'green') + " "
                #elif type(pseudo_car) == Collision:
                elif isinstance(pseudo_car, Collision):
                    string += " " + ANSI.colored_str(str(pseudo_car.countdown), 'red') + " "
            string += "\n"
        return string

    def create_output(self, index):
        '''
            This function creates a file called 'index.txt' and writes the data of the road to it
            This file will be extracted by the ETL process
        '''
        output = Output()
        output.write(f"{self.name} {index}$")
        # output.write(f"> {self.name}\n")
        output.write(f"{int(datetime.datetime.now().timestamp() * 1000)}\n")
        # output.write(f"> {index}\n")

        for lane in range(self.lanes_f):
            for length in range(self.length):
                if isinstance(self.road[lane][length], Car):
                    output.write(f"{self.road[lane][length].plate} {str(lane).zfill(3)},{length:03}\n")
                elif isinstance(self.road[lane][length], Collision):
                    for c in self.road[lane][length].collided_cars:
                        output.write(f"{c.plate} {str(lane).zfill(3)},{length:03}\n")
        
        for lane in range(self.lanes_f, self.lanes_total):
            for length in range(self.length):
                i_length = self.length - length - 1 # inverts the position of the car
                if isinstance(self.road[lane][length], Car):
                    output.write(f"{self.road[lane][length].plate} {str(lane).zfill(3)},{i_length:03}\n")
                elif isinstance(self.road[lane][length], Collision):
                    for c in self.road[lane][length].collided_cars:
                        output.write(f"{c.plate} {str(lane).zfill(3)},{i_length:03}\n")
        
        send_message(output.message)

    def loop(self):
        '''
            Calls cycle, prints data, waits for an input to continue
            Loops while true
        '''

        while True:

            self.cycle()
            os.system('cls')
            print(self)
            self.create_output(0)
            self.create_output(0)

            if self.counter > 0:
                self.counter -= 1
            else:
                command = input()
                while command != '':
                    exec(command)
                    command = input()

    def path_is_empty(self, lane, length_start, length_end) -> tuple[bool, int]:
        '''
            Returns whether a path is empty or not and the maximum length reached.
        '''
        # pylint: disable=invalid-name

        l = self.length - 1

        for l in range(length_start, length_end + 1):
            # Check whether the length is already out of the road
            if l >= self.length:
                return True, self.length - 1

            # Check whether length_n is empty
            if not self.is_empty(lane, l):
                return False, l

        return True, l

class World():
    #roads = []
    road = None

    def add_road(self, road:Road):
        #self.roads.append(road)
        self.road = road
    
    def loop(self, cycles:int=None):
        i= 0
        if cycles is None:
            while True:
                self.road.cycle()
                self.road.create_output(i)
                os.rename(f"{output_folder}/temp{i}.txt", f"{output_folder}/{i}.txt")  # Rename file to standard name
                print(f"Cycle {i} done", end='\r')
                i+=1
        else:
            for i in range(cycles):
                self.road.cycle()
                self.road.create_output(i)
                print(f"Cycle {i} done", (1+(i%3))*".", (3-(i%3))*" ", end='\r')
                # debug mode, do not delete
                # new = True
                #for road in self.roads:
                #     if new:
                #         print(road, end='\r')
                #         new = False
                    #road.cycle()
                    #road.create_output(i)
            print(" ==== DONE ==== ")

def create_world(filename:str):
    '''
        Creates a world from a file.
    '''
    world = World()
    num_lines = sum(1 for _ in open(filename))

    if not sys.argv[1].isnumeric():
        road = input(f"Which road do you want to simulate? [0 - {num_lines-1}]: ")
    else:
        road = sys.argv[1]

    assert road.isnumeric(), "Index must be a number"
    road = int(road)
    assert road >= 0 and road < num_lines, "Index out of range"

    with open(filename, 'r', encoding='utf-8') as file:
        for i, line in enumerate(file):
            if i == road:
                attr = line.split(' ')
                name = attr[0]
                lanes_f = int(attr[1])
                lanes_b = int(attr[2])
                length = int(attr[3])
                speed_limit = int(attr[4])
                prob_of_new_car = float(attr[5])/100
                prob_of_changing_lane = float(attr[6])/100
                prob_of_collision = float(attr[7])/100
                car_speed_min = int(attr[8])
                car_speed_max = int(attr[9])
                car_acc_min = int(attr[10])
                car_acc_max = int(attr[11])
                collision_fix_time = int(attr[12])

                # create road
                road = Road(name, lanes_f, lanes_b, length, speed_limit, prob_of_new_car, prob_of_changing_lane, prob_of_collision, car_speed_min, car_speed_max, car_acc_min, car_acc_max, collision_fix_time)

                # add road to world
                world.add_road(road)
                break   
    return world

def empty_roads_dir():
    run = True
    i = 0
    while run:
        if os.path.exists(f'roads/{i}.txt'):
            os.remove(f'roads/{i}.txt')
            i += 1
        else:
            run = False

if __name__ == "__main__":
    empty_roads_dir()
    world = create_world('etl/world.txt')
    world.loop(10000)
