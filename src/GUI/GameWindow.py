import sys
import os
try:
    import pygame

except ImportError:
    current_dir = os.path.dirname(os.path.abspath(__file__))
    libdeps_path = os.path.join(current_dir, '..', 'libdeps')

    if libdeps_path not in sys.path:
        sys.path.insert(0, libdeps_path)

    import pygame
from src.GAME.game import *

class GameWindow:
    def __init__(self) -> None:
        self.width:int = 700
        self.height:int = 900
        self.texture:pygame.Surface = pygame.Surface((self.width,self.height))
        self.cached:pygame.Surface = self.texture
        self.aspectRatio:float = self.width/self.height
        self.gameHandler:Game = Game((self.width,self.height))
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
