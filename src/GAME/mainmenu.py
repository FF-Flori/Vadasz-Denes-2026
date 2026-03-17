import pygame
from src.GAME.game import *

class MainMenu:
    def __init__(self,windowsize:tuple[int,int])->None:
        self.texture:pygame.Surface = pygame.transform.scale(pygame.image.load("./src/img/menu.png"),windowsize)
        self.width:int = windowsize[0]
        self.height:int = windowsize[1]

        self.buttonwidth:int = int(windowsize[0]*0.5) 
        self.buttonheight:int = int(self.buttonwidth/3)
        self.buttonPos:list[int] = [(windowsize[0]-self.buttonwidth)//2,int(windowsize[1]*0.7)]
        self.button:pygame.Surface = pygame.Surface((self.buttonwidth,self.buttonheight)).convert_alpha()
        self.normButton:tuple[float,float,float,float] = (self.buttonPos[0]/self.width,self.buttonPos[1]/self.height,self.buttonwidth/self.width,self.buttonheight/self.height)
        self.button.fill((0,0,0,0))
        pygame.draw.rect(self.button,(0, 0, 0),(0,0,self.buttonwidth,self.buttonheight),0,20)
        pygame.draw.rect(self.button,(255, 157, 111),(0,0,self.buttonwidth,self.buttonheight),3,20)

        buttontext:pygame.Surface = pygame.font.Font(None,50).render("Indítás",True,(255, 207, 161))
        self.button.blit(buttontext,((self.buttonwidth-buttontext.get_width())//2,(self.buttonheight-buttontext.get_height())//2))

    def update(self,windowDimensions:tuple[int,int,int,int])->int:
        if pygame.mouse.get_pressed(3)[0]:
            pos = pygame.mouse.get_pos()
            pos = (pos[0]-windowDimensions[0],pos[1]-windowDimensions[1])
            normalized:tuple[float,float] = (pos[0]/windowDimensions[2],pos[1]/windowDimensions[3])
            if normalized[0] >= self.normButton[0] and \
               normalized[0] <= self.normButton[0]+self.normButton[2] and \
               normalized[1] >= self.normButton[1] and \
               normalized[1] <= self.normButton[1]+self.normButton[3]:
                   return 1
        return 0
    def show(self,screen:pygame.Surface)->None:
        screen.blit(self.texture,(0,0))
        screen.blit(self.button,(self.buttonPos[0],self.buttonPos[1]))
