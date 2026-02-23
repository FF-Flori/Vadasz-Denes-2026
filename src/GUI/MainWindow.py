import pygame
from src.GUI.GameWindow import *

class MainWindow:
    def __init__(self,width:int, height:int, caption:str)->None:
        self.width = width
        self.height = height
        pygame.init()
        self.screen = pygame.display.set_mode((width,height),pygame.RESIZABLE)
        pygame.display.set_caption(caption)
        self.clock = pygame.time.Clock()
        self.running = True
        self.game:GameWindow = GameWindow(width,height)
    def mainloop(self)->None:
        while self.running:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False

            self.screen.fill("black")

            self.game.showScreen(self.screen)
            pygame.display.flip()

            self.clock.tick(60)
        return
    def quit(self)->None:
        pygame.quit()
