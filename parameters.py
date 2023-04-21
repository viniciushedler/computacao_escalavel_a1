import random

# Road parameters
# set LANES to None to use a random integer in [1,10)
LANES_F: int = 3
LANES_B: int = 2
SPEED_LIMIT: int = 10
COLLISION_COUNTDOWN: int = 9
CAR_SPAWN_PROB: float = 0.2


# Car parameters

MODELS = {
    "beetle": {
        "SPEED_MIN" : 2,
        "SPEED_MAX" : 4,
        "ACCELERATION_MIN" : -1,
        "ACCELERATION_MAX" : 1,      
    },
    "voyage": {
        "SPEED_MIN" : 2,
        "SPEED_MAX" : 6,
        "ACCELERATION_MIN" : -2,
        "ACCELERATION_MAX" : 2,      
    },
    "nivus": {
        "SPEED_MIN" : 2,
        "SPEED_MAX" : 8,
        "ACCELERATION_MIN" : -3,
        "ACCELERATION_MAX" : 4,      
    },
}

RISK: float = 0.5
SPEED_MIN: int = 3
SPEED_MAX: int = 6
ACCELERATION_MIN: int = -2
ACCELERATION_MAX: int = 2
LANE_CHANGE_PROB: float = 0.3