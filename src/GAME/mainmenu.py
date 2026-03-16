import pygame
from src.GAME.game import *

class MainMenu:
    def __init__(self,windowsize:tuple[int,int])->None:
        self.texture:pygame.Surface = pygame.Surface(windowsize)
        self.texture.fill((100,125,255))
        self.buttonPos:list[int] = [100,100]
        self.button:pygame.Surface = pygame.Surface((200,40))
    def update(self,gameHandler)->None:
        if pygame.mouse.get_pressed(3)[0]:
            pos = pygame.mouse.get_pos()
            if pos[0] >= self.buttonPos[0] and pos[0] <= self.button.get_width()+self.buttonPos[0] and pos[1] >= self.buttonPos[1] and pos[1] <= self.button.get_height()+self.buttonPos[1]:
                gameHandler.inMenu = False
    def show(self,screen:pygame.Surface)->None:
        screen.blit(self.texture,(0,0))
        screen.blit(self.button,(self.buttonPos[0],self.buttonPos[1]))