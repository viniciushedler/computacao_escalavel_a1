import random
import os
from ansi import ANSI
from typing import Union


class Collision():

    def __init__(self, collided_cars: list['Car'], countdown: int):
        self.collided_cars = collided_cars
        self.countdown = countdown
        self.lane, self.length = self.pos = collided_cars[0].pos
    
    def add(self, car):
        self.collided_cars.append(car)

class Car():

    def __init__(self, road: 'Road', lane: int):
        
        # Road
        self.road = road

        # Car parameters
        self.risk = 0.5
        self.speed_min = 3
        self.speed_max = 6
        self.acceleration_max = 2
        self.acceleration_min = -2
        self.lange_change_prob = 0.2

        # Car variables
        self._lane, self._length = self._pos = (lane, 0)
        self.speed = self.speed_min
        self.collided = False


    @property
    def pos(self):
        return self._pos

    @pos.setter
    def pos(self, new_value: tuple[int]):
        
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
        return self._lane
    
    @lane.setter
    def lane(self, new_value):

        # Saves the new position of the car
        self._lane = new_value
        # Saves in self.pos as well
        if self.pos[0] != new_value:
            self.pos = new_value, self.length
    

    @property
    def length(self):
        return self._length
    
    @length.setter
    def length(self, new_value):

        # Saves the new position of the car
        self._length = new_value
        # Saves in self.pos as well
        if self.pos[1] != new_value:
            self.pos = self.lane, new_value


    def accelerate(self):

        # Chooses how much to accelerate
        acc = random.choice([-1, 0, 1, 2])

        # Accelerates
        self.speed += acc

        # If speed is below min or above max, correct it
        if self.speed < self.speed_min:
            self.speed = self.speed_min
        elif self.speed > self.speed_max:
            self.speed = self.speed_max
    

    def change_lane(self, force = False):
        
        available_lanes = []
        
        # If can turn left
        if self.lane > 0 and (force or self.road.is_empty(self.lane-1, self.length)):
            available_lanes.append(self.lane-1)
        # If can turn right
        if self.lane < (self.road.lanes - 1) and (force or self.road.is_empty(self.lane + 1, self.length)):
            available_lanes.append(self.lane+1)

        # Returns if no lane is available
        if len(available_lanes) == 0:
            return False

        # Moves to a new lane
        self.lane = random.choice(available_lanes)
        
        return True
    
    
    def decide_movement(self):
        # Some possibilities:
        # Nothing is on the way: move
        # Something is on the way:
        #   Try to change lane:
        #       Changed lanes into something: Collision
        #       Changed Freely
        #   then, decide to go ahead
        #       Nothing is on the way: move
        #       Something is on the way: deaccelerate
        # At the end, independently, decide whether to change lanes (again)

        # 'free' variable, will decide whether to move straight or do something else
        free = True

        # Check if the path is empty from it's current position to it's current_position + speed
        free, length_new = self.road.path_is_empty(self.lane, self.length +1, self.length + self.speed)
            
        # Nothing was on the way or wanted to collide: move forward
        if free or random.random() > 1 - self.risk:
            self.length = length_new
            return
        # The following code only executes if the car didn't move already

        # Tries to change lanes, and if it fails, deaccelerates
        if not self.change_lane():
            self.speed += self.acceleration_min

        # Now, with new lane/speed (or not), gets maximum length traveled
        _, length_new = self.road.path_is_empty(self.lane, self.length +1, self.length + self.speed)
        self.length = length_new



class Road():

    def __init__(self):

        # Road parameters
        self.length = 40
        self.lanes = 4
        self.road: list[Union[Car, Collision, None]] = [ [None] * self.length for _ in range(self.lanes) ]
        print(f'Lanes: {len(self.road)}')
        print(f'Length: {len(self.road[0])}')
        self.collision_countdown = 9
        self.car_spawn_prob = 0.2

        # Road variables
        self.collisions: list[Collision] = []
        self.cycle_total = 0
        self.collision_total = 0
    

    def create_cars(self):
        
        # For every lane, choose whether to spawn a car
        for lane in range(self.lanes):
            if random.random() > 1 - self.car_spawn_prob and self.road[lane][0] == None:
                self.road[lane][0] = Car(self, lane)
    

    def is_empty(self, lane: int, length:int) -> bool:
        return (self.road[lane][length] == None)


    def move(self, position_old, position_new):
        # This function EFFECTIVELY MOVES the car inside it's register
        # There are four possibilities
        #   position_new is beyond the end of the road: just pass (will remove car at the end)
        #   position_new is empty: moves the car normally
        #   position_new contains a car: creates a collision
        #   position_new contains a collision: adds the car to the collision
        # After any of these, the car is removed from it's old position
        
        # Create some variables for readability
        lane_o, length_o = position_old
        lane_n, length_n = position_new
        object_o = self.road[lane_o][length_o]
        
        if length_n >= self.length:
            print(f'case 0: old {position_old} new {position_new}')
            self.road[lane_o][length_o] = None
            return

        object_n = self.road[lane_n][length_n]
        # Check if new position is beyond the road

        # Check if new position is empty
        if self.is_empty(lane_n, length_n):
            print(f'case 2: old {position_old} new {position_new}')
        # elif True:
            # Creates a copy of the car in the new position
            self.road[lane_n][length_n] = object_o
        
        # Check if the position contains another car
        elif type(object_n) == Car:
            print(f'case 3: old {position_old} new {position_new}')
            # Support variable
            # Note: it is important that the new position car comes first in the list
            # This is because as of this moment the old position car still has it's old position saved in it
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
        elif type(object_n) == Collision:
            print(f'case 4: old {position_old} new {position_new}')
            self.road[lane_n][length_n].add(object_o)
            self.collision_total += 1
        
        # If didn't fall into any condition before, raise an error
        else:
            raise ValueError(f"Value at ({lane_n}, {length_n}) is {type(object_n)}. Expected None, Car or Collision")

        # Removes the occurence of the car in the old position
        self.road[lane_o][length_o] = None
    

    def all_decide_movement(self):
        for length in range(self.length, -1, -1):
            for lane in range(self.lanes):
                pseudo_car = self.road[lane][length]
                if type(pseudo_car) == Car:
                    pseudo_car.decide_movement()
    

    def remove_collisions(self):
        for collision in self.collisions:
            collision.countdown -= 1
            if collision.countdown <= 0:
                if type(self.road[collision.lane][collision.length]) == Collision:
                    self.road[collision.lane][collision.length] = None


    def cycle(self):
        self.remove_collisions()

        for length in range(self.length - 1, -1, -1):
            for lane in range(self.lanes):
                pseudo_car = self.road[lane][length]
                if type(pseudo_car) == Car:
                    pseudo_car.accelerate()
                    pseudo_car.decide_movement()
        
        self.create_cars()
        self.cycle_total += 1


    def __str__(self):
        string = ""
        for lane in self.road:
            for pseudo_car in lane:
                if pseudo_car == None:
                    string += " . "
                elif type(pseudo_car) == Car:
                    string += " " + ANSI.colored_str(str(pseudo_car.speed), 'green') + " "
                elif type(pseudo_car) == Collision:
                    string += " " + ANSI.colored_str(str(pseudo_car.countdown), 'red') + " "
            string += "\n"
        return string


    def loop(self):
        while True:
            #os.system('cls')
            self.cycle()
            print(self)
            print(f"Cycles: {self.cycle_total}")
            print(f"Collisions: {self.collision_total}")
            input()


    def path_is_empty(self, lane, length_start, length_end) -> tuple[bool, int]:
        # returns whether the path is empty and the maximum length reached

        for l in range(length_start, length_end+1):
            
            # Check whether the length is already out of the road
            if l >= self.length:
                return True, l
            
            # Check whether length_n is empty
            if not self.is_empty(lane, l):
                return False, l
        
        return True, l


if __name__ == "__main__":
    r = Road()
    r.loop()