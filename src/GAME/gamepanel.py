import pygame
from src.GAME.gamelogic import *
from math import ceil

class GamePanel:
    def __init__(self,windowsize:tuple[int,int])->None:
        # the panel sprite has a margin of 20 and a width of 340
        # 20/340 = margin/windowsize[0]
        self.margin:float = ceil(windowsize[0]/17)
        self.width:int = ceil(windowsize[0]-2*self.margin)
        self.margin = ceil(self.margin)
        self.texture:pygame.Surface = pygame.Surface((self.width,self.width))
        self.logicModule:GameLogic = GameLogic(self.width,self.width)
        print(self.width)
    def updatePanel(self, deltaTime:float)->None:
        self.logicModule.Update(deltaTime,self.texture)
    def showPanel(self, screen:pygame.Surface)->None:
        screen.blit(self.texture,(self.margin,self.margin))
