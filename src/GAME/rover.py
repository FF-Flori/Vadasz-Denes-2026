import pygame
class Rover:
    def __init__(self,mapin:list[list[str]])->None:
        self.spritewidth:int = 64
        self.scaledwidth:float = self.spritewidth
        self.timer:float = 0
        self.speed:float = 2 # tiles/second
        self.frametime:float = 0.150
        self.framenum:int = 0
        self.framecount:int = 12
        self.target:list[int] = [-1,-1]
        self.pos:list[float] = [0,0]
        self.sprite:pygame.Surface = pygame.image.load("./src/img/rover.png").convert_alpha()
        self.scaled:pygame.Surface = self.sprite
        self.map:list[list[str]] = mapin
        self.battery:int = 100
        self.gear:int = 0
        self.animtype:int = 0 #Which way it will go
        for y in range(len(mapin)):
            for x in range(len(mapin[y])):
                if mapin[y][x] == 'S':
                    self.pos = [x,y]
                    mapin[y][x] = ' '
                    return
    def setGear(self,gear:int)->None:
        if gear == 0:
            self.speed = 0
        self.speed = gear*0.5
        self.gear = gear
    # This will throw an error if its impossible
    def moveTo(self,xdisp:int, ydisp:int)->None:
        self.target[0] = int(self.pos[0]+xdisp)
        self.target[1] = int(self.pos[1]+ydisp)
        if ydisp < 0:
            self.animtype = 1
        elif ydisp > 0:
            self.animtype = 3

        if xdisp > 0:
            self.animtype = 0
        elif xdisp < 0:
            self.animtype = 2

        assert(self.target[0] > -1)
        assert(self.target[1] > -1)
        assert(self.target[0] < len(self.map[0]))
        assert(self.target[1] < len(self.map))
    #0 is x 1 is y
    def _moveTowards_(self,axis:int,deltaTime:float)->None:
        if self.target[axis] < self.pos[axis]:
            self.pos[axis] -= self.speed*deltaTime
            if self.target[axis] >= self.pos[axis]:
                self.pos[axis] = self.target[axis]
                self.target[axis] = -1
        elif self.target[axis] > self.pos[axis]:
            self.pos[axis] += self.speed*deltaTime
            if self.target[axis] <= self.pos[axis]:
                self.pos[axis] = self.target[axis]
                self.target[axis] = -1
        else:
            self.target[axis] = -1

    def update(self,deltaTime:float)->None:
        self.timer+=deltaTime/1000
        if self.timer >= self.frametime:
            self.framenum+=1
            self.framenum %= self.framecount
            self.timer -= self.frametime
        if self.target[0] > -1:
            self._moveTowards_(0,deltaTime/1000)
        if self.target[1] > -1:
            self._moveTowards_(1,deltaTime/1000)

    def draw(self, screen:pygame.Surface, orewidth:float, viewstart:list[float], viewwidth:int) -> None:
        if self.pos[0]-viewstart[0] > viewwidth or self.pos[1]-viewstart[1] > viewwidth:
            return
        screen.blit(self.scaled,((self.pos[0]-viewstart[0])*orewidth,(self.pos[1]-viewstart[1])*orewidth),(self.framenum*self.scaledwidth,self.animtype*self.scaledwidth,self.scaledwidth,self.scaledwidth))
