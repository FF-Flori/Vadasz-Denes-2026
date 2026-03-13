import pygame
class Rover:
    def __init__(self,mapin:list[list[str]])->None:
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
    def draw(self, screen:pygame.Surface, orewidth:float, viewstart:list[float], viewwidth:int) -> None:
        if self.pos[0]-viewstart[0] > viewwidth or self.pos[1]-viewstart[1] > viewwidth:
            return
        pygame.draw.rect(screen,'gray',((self.pos[0]-viewstart[0])*orewidth, (self.pos[1]-viewstart[1])*orewidth,orewidth,orewidth))
