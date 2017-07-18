import pygame, math, sys, serial
from pygame.locals import *

TURN_SPEED = 8
ACCELERATION = 2
BG= (0,0,0)
MAX_Y = 1024
MAX_X = 1024
MAX_FORWARD_SPEED = 5
MAX_REVERSE_SPEED = -5
SCALE_SPEED = -20

# initialize the screen with size (MAX_X, MAX_Y)
pygame.font.init()
screen = pygame.display.set_mode((MAX_X, MAX_Y))
# load picture file - Car: http://www.xnadevelopment.com/tutorials/introducingxnagamestudioexpress/Car.png / Turtle: http://www.wpclipart.com/animals/aquatic/turtle/turtle_2/cartoon_turtle.png.html
car = pygame.image.load('Car.png')
clock = pygame.time.Clock() # load clock
k_up = k_down = k_left = k_right = 0 # init key values
speed = direction = 0 # start speed & direction
position = (MAX_X/2, MAX_Y/2) # start position
font = pygame.font.SysFont('Arial', 30)

play = True
count = 5

ser = serial.Serial('/dev/ttyACM0', 9600, writeTimeout = 10)

def update_car(speed, direction):
    global ser
    try:
        speedStr = 's ' + str(speed)
        ser.write(speedStr.encode())
        directionStr = 'd ' + str(direction)
        ser.write(directionStr.encode())
    except serial.serialutil.SerialTimeoutException as err:
        print(err)
        
        ser.close()
        ser = serial.Serial('/dev/ttyACM0', 9600, writeTimeout = 10)
        print("reconnect")

def reset_car():
    update_car(280, 340)


reset_car()


while play:
    # USER INPUT
    clock.tick(30)
    count -= 1
    # get events from the user
    for event in pygame.event.get():
        # not a key event
        if not hasattr(event, 'key'):
            continue
        # check if presses a key or left it
        down = event.type == KEYDOWN # key down or up?        
        # key events: http://pygame.org/docs/ref/key.html
        if event.key == K_RIGHT:
            k_right = down * TURN_SPEED
        elif event.key == K_LEFT:
            k_left = down * TURN_SPEED
        elif event.key == K_UP:
            k_up = down * ACCELERATION
        elif event.key == K_DOWN:
            k_down = down * ACCELERATION
        elif event.key == K_ESCAPE:
            play = False            
    screen.fill(BG)
    # SIMULATION
    # .. new speed and direction based on acceleration and turn
    speed += (k_up - k_down)
    direction -= (k_right - k_left)
    if speed > 0:
        speed -= ACCELERATION/4
        if speed > MAX_FORWARD_SPEED:
            speed = MAX_FORWARD_SPEED
        if speed < 0:
            speed = 0
    elif speed < 0:
        speed += ACCELERATION/4
        if speed < MAX_REVERSE_SPEED:
            speed = MAX_REVERSE_SPEED
        if speed > 0:
            speed = 0
    if direction > 0:
        direction -= TURN_SPEED/4
        if direction > 50:
            direction = 50
        if direction < 0:
            direction = 0
    elif direction < 0:
        direction += TURN_SPEED/4
        if direction < -50:
            direction = -50
        if direction > 0:
            direction = 0
    # .. new position based on current position, speed and direction
    rad = direction * math.pi / 180

    position = (MAX_X/2, speed * SCALE_SPEED + MAX_Y/2)

    #print(position)

    # RENDERING
    # .. rotate the car image for direction
    rotated = pygame.transform.rotate(car, direction)
    # .. position the car on screen
    rect = rotated.get_rect()
    rect.center = position
    # .. render the car to screen
    screen.blit(rotated, rect)
    # render car info to screen
    dirStr = 'Direction: ' + str(direction) + ' degree'
    screen.blit(font.render(dirStr, True, (200,200,200)), (50, 50))

    speedStr = "Speed: " + str(speed)
    screen.blit(font.render(speedStr, True, (200,200,200)), (50, 100))

    pygame.display.flip()

    # Update speed and direction
    if count == 0:
        update_car(speed * 10 + 280, -direction + 340)
        count = 5

reset_car()
ser.close()
pygame.quit()
sys.exit(0) # quit the game
