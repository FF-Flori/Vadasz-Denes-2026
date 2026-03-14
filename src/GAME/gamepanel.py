import pygame
from src.GAME.gamelogic import *

class GamePanel:
    def __init__(self,windowsize:tuple[int,int])->None:
        self.margin:int = 20
        self.width:int = windowsize[0]-2*self.margin
        self.height:int = windowsize[0]-2*self.margin
        self.posOnScreen:tuple[int,int] = (windowsize[0]//2-self.width//2,self.margin)
        self.texture:pygame.Surface = pygame.Surface((self.width,self.height))
        self.logicModule:GameLogic = GameLogic(self.width,self.height)
    def updatePanel(self, deltaTime:float)->None:
        self.logicModule.Update(deltaTime,self.texture)
    def showPanel(self, screen:pygame.Surface)->None:
        screen.blit(self.texture,self.posOnScreen)
