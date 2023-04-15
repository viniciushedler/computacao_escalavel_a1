import random
import os


# about scales, we'll be simulating roads in Brazil
# so, the base parameters are as follow
# minimum speed: 30 (km/h)
# maximum speed: 60 (km/h)
# minimum acceleration: -30 (km/h /s)
# maximum acceleration: 10 (km/h /s)

# our parameters will be represented by tiles and cycles
# every tile of our matrix will represent 10km
# so, we convert our parameters:
# minimum speed: 3 (t/c)
# maximum speed: 6 (t/c)
# minimum acceleration: -3 (t/c²)
# maximum acceleration: 1 (t/c²)

# also, the minimum and maximum speeds are "recommendations"
# most drivers will stick to them, but not all
# so, the following parameters are also needed
# true minimum speed: 1 (t/c)
# true maximum speed: 10 (t/c)

# and we have a vision parameter
# the vision parameter is how many cycles the driver can foresee

class Road():

    def __init__(self):
        self.length = 100
        self.lanes = 10
        self.road = [ [0] * self.length for _ in range(self.lanes) ]
        self.removal_time = 3
        self.minimum_speed = 3
        self.maximum_speed = 6
        self.maximum_acceleration = 1
        self.minimum_acceleration = -3
        self.colision_positions = []
        self.colision_timers = []
        self.cycle_total = 0
        self.colision_total = 0

    def create_car(self, lane=None):
        if lane==None:
            lane = random.choice(range(self.lanes))
        if self.road[lane][0] == 0:
            self.road[lane][0] = 1
        else:
            print(f"Can't create car on lane {lane}")
    
    def accelerate(self, lane, length, acc=None):
        if acc==None:
            acc = random.choice([-1, 0, 1, 2])
        # doesn't do anything if there's no car in the position
        if self.road[lane][length] <= 0:
            pass
        # if the car would go above max speed, keep him at it
        elif self.road[lane][length] + acc >= self.maximum_speed:
            self.road[lane][length] = self.maximum_speed
        # if the car would go below min speed, keep him at it
        elif self.road[lane][length] + acc < self.minimum_speed:
            self.road[lane][length] = self.minimum_speed
        # in other cases, just accelerate
        else:
            self.road[lane][length] += acc

    
    def change_lane(self, lane, length):
        if lane>0:
            if self.road[lane-1][length] == 0:
                self.road[lane-1][length] = self.road[lane][length]
                self.road[lane][length] = 0
                return lane-1
        if lane<self.lanes-1:
            if self.road[lane+1][length] == 0:
                self.road[lane+1][length] = self.road[lane][length]
                self.road[lane][length] = 0
                return lane+1
        return -1

    def move(self, lane, length):
        if self.road[lane][length] <= 0:
            return
        
        free = True  

        # First try at moving the car
        for dist in range(1, self.road[lane][length]+1):

            # if the car would get out of track at this distance,
            # it just gets deleted
            if length + dist >= self.length-1:
                self.road[lane][length] = 0
                return

            # check if the road at given distance is occupied
            if self.road[lane][length + dist] != 0:
                free = False
                break
        
        # if there's nothing stopping the car from moving, it moves
        if free:
            self.road[lane][length + dist] = self.road[lane][length]
            self.road[lane][length] = 0
        
        # if it couldn't move at first, tries swapping lanes and
        # deaccelerating in order not to crash
        else:

            # tries changing lanes
            new_lane = self.change_lane(lane, length)
        
            # if changed lanes, save new_lane
            if new_lane != -1:
                lane = new_lane
            # if couldn't change lanes, tries deaccelerating
            else:
                self.accelerate(lane, length, self.minimum_acceleration)

            # now, with a new lane or speed, move even if it causes a collision
            free = True
            for dist in range(1, self.road[lane][length]+1):
                
                # check collision
                if self.road[lane][length + dist] != 0:
                    print(f'COLISION! At {lane}, {length + dist}')
                    self.road[lane][length + dist] = -1
                    self.colision_positions.append((lane, length+dist))
                    self.colision_timers.append(self.removal_time)
                    self.colision_total += 1
                    free = False
                    break
                
                # if car would get out of road...
                if length + dist >= self.length-1:
                    self.road[lane][length] = 0
                    return
            
            if free:
                # puts it in new position
                self.road[lane][length + dist] = self.road[lane][length]
            # removes the car from old position
            self.road[lane][length] = 0

    def move_all(self):
        
        for length in range(self.length, -1, -1):
            for lane in range(self.lanes):
                self.move(lane, length)
    
    def removal(self):
        i = 0
        while i<len(self.colision_positions):
            if self.colision_timers[i] > 0:
                break
            lane, length = self.colision_positions[i]
            if self.road[lane][length] == -1:
                self.road[lane][length] = 0
            self.colision_positions = self.colision_positions[1:]
            self.colision_timers = self.colision_timers[1:]
            i += 1

        while i < len(self.colision_positions):
            self.colision_timers[i] -= 1
            i += 1


    def cycle(self):
        self.removal()
         
        for length in range(self.length-1, -1, -1):
            for lane in range(self.lanes):
                self.accelerate(lane, length)
                self.move(lane, length)

        while random.random()>.3:
            self.create_car()
        
        self.cycle_total += 1

    def __str__(self):
        string = ""
        for lane in self.road:
            for car in lane:
                if car>=0:
                    string += " "
                string += str(car) + " "
            string += "\n"
        return string

    def main(self):
        while True:
            os.system('cls')
            self.cycle()
            print(self)
            print(f"Cycles: {self.cycle_total}")
            print(f"Colisions: {self.colision_total}")
            input()

r = Road()
r.main()