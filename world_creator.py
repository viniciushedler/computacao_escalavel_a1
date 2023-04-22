import random

if __name__ == "__main__":
    roads_number = 15
    lanes_number_range = [1, 3]
    max_speed_range = [12, 24]

    with open("world.txt", "w") as f:
        initial_road = 100
        for i in range(roads_number):
            f.write(f"BR-{initial_road + i} ")

            left_lanes_number = random.randint(lanes_number_range[0], lanes_number_range[1])
            right_lanes_number = random.randint(lanes_number_range[0], lanes_number_range[1])
            f.write(f"{left_lanes_number} {right_lanes_number}")

            max_speed = random.randint(max_speed_range[0], max_speed_range[1])
            f.write(f" {str(max_speed*5).zfill(3)}")

            f.write("\n")
