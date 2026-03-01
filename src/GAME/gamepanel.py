import pygame
from src.GAME.gamelogic import *

class GamePanel:
    def __init__(self) -> None:
        self.margin:int = 20
        self.width = 300
        self.height = 300
        self.aspectRatio = self.width/self.height
        self.texture:pygame.Surface = pygame.Surface((self.width,self.height))
        self.texture.fill((235,131,52))
        self.logicModule:GameLogic = GameLogic()
    def updatePanel(self, deltaTime:float)->None:
        self.texture.fill((235,131,52))
        self.logicModule.Update(deltaTime,self.texture)
    def showPanel(self, screen:pygame.Surface)->None:
        screenwidth:int = screen.get_width()
        newwidth:int = screenwidth-2*self.margin
        newheight:int = int(newwidth/self.aspectRatio)
        screen.blit(pygame.transform.scale(self.texture,(newwidth,newheight)),((screenwidth-newwidth)/2,self.margin))
