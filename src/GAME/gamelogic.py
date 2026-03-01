import pygame
class GameLogic:
    def __init__(self) -> None:
        # This is the equivalent of the Start function from unity
        self.viewedWidth:int = 25
        self.viewed:list[int] = [0,0]
        with open("./src/PATHFINDER/mars_map_50x50.csv") as file:
            self.map = file.readlines()
        for line in self.map:
            line = line[:-1].split(',')
    # deltaTime is in miliseconds
    def Update(self,deltaTime:float,screen:pygame.Surface) -> None:
        keys = pygame.key.get_pressed()
        if keys[pygame.K_a]:
            self.viewed[0] -=1
            if self.viewed[0] < 0:
                self.viewed[0] = 0
        if keys[pygame.K_w]:
            self.viewed[1] -=1
            if self.viewed[1] < 0:
                self.viewed[1] = 0
        if keys[pygame.K_s]:
            self.viewed[1] +=1
            if self.viewed[1] > len(self.map)-self.viewedWidth:
                self.viewed[1] = len(self.map)-self.viewedWidth
        if keys[pygame.K_d]:
            self.viewed[0] +=1
            if self.viewed[0] > len(self.map[0])-self.viewedWidth:
                self.viewed[0] = len(self.map[0])-self.viewedWidth
        blockwidth = screen.get_width()/self.viewedWidth
        for y in range(self.viewed[1],self.viewed[1]+self.viewedWidth):
            for x in range(self.viewed[0],self.viewed[0]+self.viewedWidth):
                if self.map[y][x] == 'Y':
                    pygame.draw.rect(screen,"yellow",((x-self.viewed[0])*blockwidth,(y-self.viewed[1])*blockwidth,blockwidth-1,blockwidth-1))
                if self.map[y][x] == 'G':
                    pygame.draw.rect(screen,"green",((x-self.viewed[0])*blockwidth,(y-self.viewed[1])*blockwidth,blockwidth-1,blockwidth-1))
                if self.map[y][x] == 'B':
                    pygame.draw.rect(screen,"blue",((x-self.viewed[0])*blockwidth,(y-self.viewed[1])*blockwidth,blockwidth-1,blockwidth-1))
                if self.map[y][x] == '#':
                    pygame.draw.rect(screen,"brown",((x-self.viewed[0])*blockwidth,(y-self.viewed[1])*blockwidth,blockwidth-1,blockwidth-1))
                if self.map[y][x] == 'S':
                    pygame.draw.rect(screen,"purple",((x-self.viewed[0])*blockwidth,(y-self.viewed[1])*blockwidth,blockwidth-1,blockwidth-1))



