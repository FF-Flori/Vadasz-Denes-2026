import pygame
from src.GAME.game import *

class GameWindow:
    def __init__(self,width:int,height:int) -> None:
        self.texture:pygame.Surface = pygame.Surface((width,height))
        self.cached:pygame.Surface = self.texture
        self.width:int = width
        self.height:int = height
        self.aspectRatio:float = width/height
        self.gameHandler:Game = Game((width,height))
    def showScreen(self,screen:pygame.Surface,deltaTime:float)->None:
        screenwidth:int = screen.get_width()
        screenheight:int = screen.get_height()

        xpos:int = 0
        ypos:int = 0
        newwidth:int = 0
        newheight:int = 0
        if self.aspectRatio > screenwidth/screenheight:
            newwidth = screenwidth
            newheight = int(screenwidth/self.aspectRatio)
            xpos = 0
            ypos = (screenheight-newheight)//2
        elif self.aspectRatio < screenwidth/screenheight:
            newwidth = int(self.aspectRatio*screenheight)
            newheight = screenheight
            xpos = (screenwidth-newwidth)//2
        else:
            newwidth = screenwidth
            newheight = screenheight


        self.gameHandler.drawGame(self.texture,deltaTime,(xpos,ypos,newwidth,newheight))


        screen.blit(pygame.transform.scale(self.texture,(newwidth,newheight)),(xpos,ypos))

        # The width is bigger than normal relative to the height
