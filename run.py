from sys import platform
import os

if __name__=="__main__":

    match platform:
        case ('win32'):
            os.system('python3 world_creator.py')
            os.system('start cmd /k python3 mock/mock.py')
            os.system('cd etl')
            os.system('g++ etl.cpp -o etl')
            os.system('./etl')
        case ('linux'):
            os.system('python3 world_creator.py; gnome-terminal -- bash -c "python3 mock/mock.py; exec bash"; cd etl; g++ etl.cpp -o etl; ./etl')