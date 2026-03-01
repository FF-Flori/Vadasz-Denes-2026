import pygame
from src.GAME.game import *

class GameWindow:
    def __init__(self,width:int,height:int) -> None:
        self.texture:pygame.Surface = pygame.Surface((width,height))
        self.width:int = width
        self.height:int = height
        self.aspectRatio:float = width/height
        self.gameHandler:Game = Game()
    def showScreen(self,screen:pygame.Surface,deltaTime:float)->None:
        screenwidth:int = screen.get_width()
        screenheight:int = screen.get_height()

        self.gameHandler.drawGame(self.texture,deltaTime)

        # The width is bigger than normal relative to the height
        if self.aspectRatio > screenwidth/screenheight:
            newwidth:int = screenwidth
            newheight:int = int(screenwidth/self.aspectRatio)
            screen.blit(pygame.transform.scale(self.texture,(newwidth,newheight)),(0,(screenheight-newheight)/2))
        elif self.aspectRatio < screenwidth/screenheight:
            newwidth:int = int(self.aspectRatio*screenheight)
            newheight:int = screenheight
            screen.blit(pygame.transform.scale(self.texture,(newwidth,newheight)),((screenwidth-newwidth)/2,0))
        else:
            screen.blit(pygame.transform.scale(self.texture,(screenwidth,screenheight)),(0,0))
