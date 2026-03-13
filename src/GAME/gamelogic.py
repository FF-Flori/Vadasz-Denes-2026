import pygame
import math

class GameLogic:
    def __init__(self, scrwidth, scrheight) -> None:
        # This is the equivalent of the Start function from unity
        self.width = scrwidth
        self.height = scrheight
        self.speed:float = 20
        self.viewed:list[float] = [0,0]
        self.oreImgs:pygame.Surface = pygame.image.load("./src/img/ores.png").convert_alpha()
        #self.oreImgs = pygame.transform.scale(self.oreImgs,(self.oreImgs.get_width()//2,self.oreImgs.get_height()//2))
        self.orewidth:int = 40
        with open("./src/PATHFINDER/mars_map_50x50.csv") as file:
            self.map = file.readlines()
        for i in range(len(self.map)):
            self.map[i] = self.map[i].strip().split(',')
        self.viewedWidth:int = self.width//self.orewidth
    # Posx and Posy store where the ore is in the map
    def drawOre(self, oreType:str, posx:float, posy:float, width:int, height:int, screen:pygame.Surface):
        oreRect = pygame.Rect(0,0,self.orewidth,self.orewidth)
        if oreType == 'Y':
            oreRect.x = self.orewidth*2
            screen.blit(self.oreImgs,(posx,posy), oreRect)
        if oreType == 'G':
            oreRect.x = self.orewidth*3
            screen.blit(self.oreImgs,(posx,posy), oreRect)
        if oreType == 'B':
            oreRect.x = self.orewidth
            screen.blit(self.oreImgs,(posx,posy), oreRect)
        if oreType == '#':
            oreRect.x = 0
            screen.blit(self.oreImgs,(posx,posy), oreRect)
        if oreType == 'S':
            pygame.draw.rect(screen,"purple",(posx,posy,width,height))
    def moveCamera(self,dispX:float,dispY:float)->None:
        self.viewed[0] += dispX
        self.viewed[1] += dispY
        if dispX != 0:
            if self.viewed[0] < 0:
                self.viewed[0] = 0
            elif self.viewed[0] > len(self.map[0])-self.viewedWidth:
                self.viewed[0] = len(self.map[0])-self.viewedWidth
        if dispY != 0:
            if self.viewed[1] < 0:
                self.viewed[1] = 0
            elif self.viewed[1] > len(self.map)-self.viewedWidth:
                self.viewed[1] = len(self.map)-self.viewedWidth
    # deltaTime is in miliseconds
    def Update(self,deltaTime:float,screen:pygame.Surface) -> None:
        keys = pygame.key.get_pressed()
        if keys[pygame.K_a]:
            self.moveCamera(-self.speed/1000*deltaTime,0)
        if keys[pygame.K_w]:
            self.moveCamera(0,-self.speed/1000*deltaTime)
        if keys[pygame.K_s]:
            self.moveCamera(0,self.speed/1000*deltaTime)
        if keys[pygame.K_d]:
            self.moveCamera(self.speed/1000*deltaTime,0)
        for y in range(int(self.viewed[1]),math.ceil(self.viewed[1]+self.viewedWidth)):
            for x in range(int(self.viewed[0]),math.ceil(self.viewed[0]+self.viewedWidth)):
                self.drawOre(self.map[y][x],(x-self.viewed[0])*self.orewidth,(y-self.viewed[1])*self.orewidth,self.orewidth,self.orewidth,screen)