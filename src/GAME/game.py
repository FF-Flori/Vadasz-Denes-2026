import pygame
from src.GAME.gamepanel import *
from src.GAME.HUD import *
from src.GAME.mainmenu import *

class Game:
    def __init__(self,windowsize:tuple[int,int]) -> None:
        self.inMenu:bool = True
        self.gameHandler:GamePanel = GamePanel(windowsize)
        self.HUDHandler:HUD = HUD(windowsize)
        self.MenuHandler:MainMenu = MainMenu(windowsize)
    def drawGame(self, screen:pygame.Surface, deltaTime:float) -> None:
        if self.inMenu:
            self.MenuHandler.update(self)
            self.MenuHandler.show(screen)
        else:
            self.gameHandler.updatePanel(deltaTime)
            self.HUDHandler.update(self.gameHandler.logicModule)
            self.HUDHandler.show(screen)
            self.gameHandler.showPanel(screen)