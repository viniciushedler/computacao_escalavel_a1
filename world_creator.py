import random

if __name__ == "__main__":
    roads_number = 15
    lanes_number_range = [1, 3]
    max_speed_range = [12, 24]
    vehicle_speed_range = [1, 3]
    vehicle_acceleration_range = [1, 3]
    prob_of_spawn_range = [0.1, 0.3]
    prob_of_changing_lane_range = [0.1, 0.3]
    prob_of_collision_range = [0.01, 0.05]
    time_to_fix_collision_range = [10, 30]

    with open("etl/world.txt", "w") as f:
        initial_road = 100
        for i in range(roads_number):
            # Nome da estrada
            f.write(f"BR-{initial_road + i} ")

            # Numero de faixas
            left_lanes_number = random.randint(lanes_number_range[0], lanes_number_range[1])
            right_lanes_number = random.randint(lanes_number_range[0], lanes_number_range[1])
            f.write(f"{left_lanes_number} {right_lanes_number}")
            
            # Velocidade máxima da estrada
            max_speed = random.randint(max_speed_range[0], max_speed_range[1])
            f.write(f" {str(max_speed*5).zfill(3)}")

            # Velocidade e aceleração dos veículos
            vehicle_speed = random.randint(vehicle_speed_range[0], vehicle_speed_range[1])
            vehicle_acceleration = random.randint(vehicle_acceleration_range[0], vehicle_acceleration_range[1])
            f.write(f" {str(vehicle_speed*5).zfill(3)} {str(vehicle_acceleration*5).zfill(3)}")

            # Probabilidade de spawn de veículos
            prob_of_spawn = random.uniform(prob_of_spawn_range[0], prob_of_spawn_range[1])
            prob_of_spawn = round(prob_of_spawn, 2)
            f.write(f" {str(prob_of_spawn).zfill(4)}")

            # Probabilidade de mudança de faixa
            prob_of_changing_lane = random.uniform(prob_of_changing_lane_range[0], prob_of_changing_lane_range[1])
            prob_of_changing_lane = round(prob_of_changing_lane, 2)
            f.write(f" {str(prob_of_changing_lane).zfill(4)}")

            # Probabilidade de colisão
            prob_of_collision = random.uniform(prob_of_collision_range[0], prob_of_collision_range[1])
            prob_of_collision = round(prob_of_collision, 2)
            f.write(f" {str(prob_of_collision).zfill(4)}")

            # Tempo para consertar uma colisão
            time_to_fix_collision = random.randint(time_to_fix_collision_range[0], time_to_fix_collision_range[1])
            f.write(f" {str(time_to_fix_collision)}")

            f.write("\n")
