import pygame
from src.GAME.gamepanel import *
from src.GAME.HUD import *
from src.GAME.mainmenu import *
from src.GAME.simtimeentry import *

class Game:
    def __init__(self,windowsize:tuple[int,int]) -> None:
        self.menu:int = 0
        self.gameHandler:GamePanel = GamePanel(windowsize)
        self.HUDHandler:HUD = HUD(windowsize)
        self.MenuHandler:MainMenu = MainMenu(windowsize)
        self.entryHandler:Question = Question(windowsize)
    def drawGame(self, screen:pygame.Surface, deltaTime:float,windowDimensions:tuple[int,int,int,int]) -> None:
        if self.menu == 0:
            self.menu = self.MenuHandler.update(windowDimensions)
            self.MenuHandler.show(screen)
        elif self.menu == 1:
            if self.entryHandler.update(windowDimensions):
                self.menu += 1
                self.gameHandler.logicModule.SetTimeValue(self.entryHandler.simtime*30)
                self.HUDHandler.setTime = self.entryHandler.simtime*30
            self.entryHandler.show(screen)
        else:
            self.gameHandler.updatePanel(deltaTime)
            self.HUDHandler.update(self.gameHandler.logicModule)
            self.HUDHandler.show(screen)
            self.gameHandler.showPanel(screen)
