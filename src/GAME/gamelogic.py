import pygame
from math import floor, ceil
from random import randint
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
        self.orewidth:float = 64
        self.map:list[list[str]] = []
        with open("./src/PATHFINDER/mars_map_50x50.csv") as file:
            for row in file.readlines():
                self.map.append(row.strip().split(','))
        self.viewedWidth:int = self.width//self.orewidth
        self.rover:Rover = Rover(self.map)
        self.zoom:float = 0.5
        self.createBG()
        self.scaledBG:pygame.Surface = self.background
        self.scale()
    def createBG(self)->None:
        source:pygame.Surface = pygame.image.load("./src/img/bg.png").convert_alpha()
        tilesize:int = 64
        self.background:pygame.Surface = pygame.Surface((tilesize*len(self.map),tilesize*len(self.map[0])))

        srcRect:pygame.Rect = pygame.Rect(0,0,tilesize,tilesize)
        for x in range(len(self.map)):
            for y in range(len(self.map[0])):
                randnum:int = randint(0,99)
                # 80% for the topleft
                # 10% for the topright
                # 7% for the bottomleft
                # 3% for the bottomright
                if randnum > -1 and randnum < 80:
                    srcRect.x = 0
                    srcRect.y = 0
                elif randnum < 90:
                    srcRect.x = tilesize
                    srcRect.y = 0
                elif randnum < 97:
                    srcRect.x = 0
                    srcRect.y = tilesize
                else:
                    srcRect.x = tilesize
                    srcRect.y = tilesize
                self.background.blit(source,(x*tilesize,y*tilesize),srcRect)

    # Posx and Posy store where the ore is in the map
    def drawOre(self, oreType:str, posx:float, posy:float, screen:pygame.Surface):
        oreRect = pygame.Rect(0,0,self.orewidth,self.orewidth)
        if oreType == 'Y':
            oreRect.x = int(self.orewidth*2)
            screen.blit(self.oreImgs,(posx,posy), oreRect)
        if oreType == 'G':
            oreRect.x = int(self.orewidth*3)
            screen.blit(self.oreImgs,(posx,posy), oreRect)
        if oreType == 'B':
            oreRect.x = int(self.orewidth)
            screen.blit(self.oreImgs,(posx,posy), oreRect)
        if oreType == '#':
            oreRect.x = 0
            screen.blit(self.oreImgs,(posx,posy), oreRect)
    def scale(self):
        if self.zoom > 5:
            self.zoom = 5
        if self.zoom < 0.3:
            self.zoom = 0.3
        center:list[float] = [self.viewed[0]+self.viewedWidth/2,self.viewed[1]+self.viewedWidth/2]
        self.orewidth = self.oreSrc.get_height()*self.zoom
        neworewidth = self.oreSrc.get_height()*self.zoom
        if ceil(self.width//neworewidth) >= len(self.map):
            self.zoom += 0.1
            return
        self.viewedWidth = self.width//self.orewidth
        self.oreImgs = pygame.transform.scale(self.oreSrc,(self.oreSrc.get_width()*self.zoom,self.oreSrc.get_height()*self.zoom))
        self.rover.scaled = pygame.transform.scale(self.rover.sprite,(self.rover.sprite.get_width()*self.zoom,self.rover.sprite.get_height()*self.zoom))
        self.rover.scaledwidth = self.rover.spritewidth*self.zoom
        self.moveCamera(center[0]-self.viewedWidth/2-self.viewed[0],center[1]-self.viewedWidth/2-self.viewed[1])
        self.scaledBG = pygame.transform.scale(self.background,(self.background.get_width()*self.zoom,self.background.get_height()*self.zoom))
    def moveCamera(self,dispX:float,dispY:float)->None:
        self.viewed[0] += dispX
        self.viewed[1] += dispY
        if dispX != 0:
            if self.viewed[0] < 0:
                self.viewed[0] = 0
            elif ceil(self.viewed[0]+self.viewedWidth) > len(self.map[0]):
                self.viewed[0] = len(self.map[0])-self.viewedWidth-0.3
        if dispY != 0:
            if self.viewed[1] < 0:
                self.viewed[1] = 0
            elif ceil(self.viewed[1]+self.viewedWidth) > len(self.map):
                self.viewed[1] = len(self.map)-self.viewedWidth-0.3

    # deltaTime is in miliseconds
    def Update(self,deltaTime:float,screen:pygame.Surface) -> None:
        self.rover.update(deltaTime)
        keys = pygame.key.get_pressed()
        if keys[pygame.K_a] or keys[pygame.K_LEFT]:
            self.moveCamera(-self.speed/1000*deltaTime,0)
        if keys[pygame.K_w] or keys[pygame.K_UP]:
            self.moveCamera(0,-self.speed/1000*deltaTime)
        if keys[pygame.K_s] or keys[pygame.K_DOWN]:
            self.moveCamera(0,self.speed/1000*deltaTime)
        if keys[pygame.K_d] or keys[pygame.K_RIGHT]:
            self.moveCamera(self.speed/1000*deltaTime,0)
        if keys[pygame.K_p]:
            print(self.zoom)

        screen.blit(self.scaledBG,(0,0),(self.viewed[0]*self.orewidth,self.viewed[1]*self.orewidth,self.width,self.height))

        for y in range(int(self.viewed[1]),ceil(self.viewed[1]+self.viewedWidth)):
            for x in range(int(self.viewed[0]),ceil(self.viewed[0]+self.viewedWidth)):
                self.drawOre(self.map[y][x],(x-self.viewed[0])*self.orewidth,(y-self.viewed[1])*self.orewidth,screen)

        self.rover.draw(screen,self.orewidth,self.viewed,self.viewedWidth)
