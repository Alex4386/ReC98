INPUT_UP = 1
INPUT_DOWN = 2
INPUT_LEFT = 4
INPUT_RIGHT = 8
INPUT_SHOT = 10h
INPUT_BOMB = 20h
INPUT_CANCEL = 40h
INPUT_OK = 80h
INPUT_Q = 100h
INPUT_UP_LEFT = 1000h
INPUT_UP_RIGHT = 2000h
INPUT_DOWN_LEFT = 4000h
INPUT_DOWN_RIGHT = 8000h

INPUT_MOVEMENT_ALIGNED = (INPUT_UP or INPUT_DOWN or INPUT_LEFT or INPUT_RIGHT)
INPUT_MOVEMENT_DIAGONAL = (INPUT_UP_LEFT or INPUT_UP_RIGHT or INPUT_DOWN_LEFT or INPUT_DOWN_RIGHT)
