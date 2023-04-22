import random

if __name__ == "__main__":
    roads_number = 15
    lanes_number_range = [1, 3]
    max_speed_range = [12, 24]
    prob_of_new_car_range = [0.1, 0.3]
    prob_of_changing_lane_range = [0.1, 0.3]
    prob_of_collision_range = [0.01, 0.05]
    car_speed_range = [12, 24]
    car_acc_range = [-3, 3]
    collision_fix_time_range = [10, 30]


    with open("etl/world.txt", "w") as f:
        initial_road = 100
        for i in range(roads_number):
            f.write(f"BR-{initial_road + i} ")

            left_lanes_number = random.randint(lanes_number_range[0], lanes_number_range[1])
            right_lanes_number = random.randint(lanes_number_range[0], lanes_number_range[1])
            f.write(f"{left_lanes_number} {right_lanes_number}")

            max_speed = random.randint(max_speed_range[0], max_speed_range[1])
            f.write(f" {str(max_speed*5).zfill(3)}")

            prob_of_new_car = random.uniform(prob_of_new_car_range[0], prob_of_new_car_range[1])
            f.write(f" {str(int(prob_of_new_car*100)).zfill(2)}")

            prob_of_changing_lane = random.uniform(prob_of_changing_lane_range[0], prob_of_changing_lane_range[1])
            f.write(f" {str(int(prob_of_changing_lane*100)).zfill(2)}")

            prob_of_collision = random.uniform(prob_of_collision_range[0], prob_of_collision_range[1])
            f.write(f" {str(int(prob_of_collision*100)).zfill(2)}")

            car_speed_min = car_speed_range[0]
            car_speed_max = car_speed_range[1]
            f.write(f" {str(car_speed_min*5).zfill(3)} {str(car_speed_max*5).zfill(3)}")

            car_acc_min = car_acc_range[0]
            car_acc_max = car_acc_range[1]
            f.write(f" {str(car_acc_min).zfill(3)} {str(car_acc_max).zfill(3)}")

            collision_fix_time = random.randint(collision_fix_time_range[0], collision_fix_time_range[1])
            f.write(f" {str(collision_fix_time).zfill(2)}")

            f.write("\n")
