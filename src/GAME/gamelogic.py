import pygame
import math
from src.GAME.rover import *

class GameLogic:
    def __init__(self, scrwidth, scrheight) -> None:
        # This is the equivalent of the Start function from unity
        self.width = scrwidth
        self.height = scrheight
        self.speed:float = 20
        self.viewed:list[float] = [0,0]
        self.oreSrc:pygame.Surface = pygame.image.load("./src/img/ores.png").convert_alpha()
        self.oreImgs:pygame.Surface = self.oreSrc
        self.orewidth:float = 40
        with open("./src/PATHFINDER/mars_map_50x50.csv") as file:
            self.map = file.readlines()
        for i in range(len(self.map)):
            self.map[i] = self.map[i].strip().split(',')
        self.viewedWidth:int = self.width//self.orewidth
        self.rover:Rover = Rover(self.map)
        self.zoom:float = 1
    # Posx and Posy store where the ore is in the map
    def drawOre(self, oreType:str, posx:float, posy:float, screen:pygame.Surface):
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
    # factor is in reference of the original, not the already scaled one
    def scale(self):
        self.oreImgs = pygame.transform.scale(self.oreSrc,(self.oreSrc.get_width()*self.zoom,self.oreSrc.get_height()*self.zoom))
        self.orewidth = self.oreSrc.get_height()*self.zoom
        self.viewedWidth = self.width//self.orewidth
        if self.viewed[0]+self.viewedWidth >= len(self.map):
            self.viewed[0] = len(self.map)-self.viewedWidth
        if self.viewed[1]+self.viewedWidth >= len(self.map[0]):
            self.viewed[1] = len(self.map[0])-self.viewedWidth
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
                self.drawOre(self.map[y][x],(x-self.viewed[0])*self.orewidth,(y-self.viewed[1])*self.orewidth,screen)
        self.rover.draw(screen,self.orewidth,self.viewed,self.viewedWidth)