import pygame
class Rover:
    def __init__(self,mapin:list[list[str]])->None:
        self.spritewidth:int = 64
        self.scaledwidth:float = self.spritewidth
        self.timer:float = 0
        self.frametime:float = 0.150
        self.framenum:int = 0
        self.framecount:int = 12
        self.sprite:pygame.Surface = pygame.image.load("./src/img/rover.png").convert_alpha()
        self.scaled:pygame.Surface = self.sprite
        self.map:list[list[str]] = mapin
        for y in range(len(mapin)):
            for x in range(len(mapin[y])):
                if mapin[y][x] == 'S':
                    self.pos = [x,y]
                    mapin[y][x] = ' '
    # This will throw an error if its impossible
    def move(self,xdisp:int, ydisp:int)->None:
        self.pos[0] += xdisp
        self.pos[1] += ydisp
        assert(self.pos[0] > 0)
        assert(self.pos[1] > 0)
        assert(self.pos[0] < len(self.map[0]))
        assert(self.pos[1] < len(self.map))
    def update(self,deltaTime:float)->None:
        self.timer+=deltaTime/1000
        if self.timer >= self.frametime:
            self.framenum+=1
            self.framenum %= self.framecount
            self.timer -= self.frametime

    def draw(self, screen:pygame.Surface, orewidth:float, viewstart:list[float], viewwidth:int) -> None:
        if self.pos[0]-viewstart[0] > viewwidth or self.pos[1]-viewstart[1] > viewwidth:
            return
        screen.blit(self.scaled,((self.pos[0]-viewstart[0])*orewidth,(self.pos[1]-viewstart[1])*orewidth),(self.framenum*self.scaledwidth,0,self.scaledwidth,self.scaledwidth))
