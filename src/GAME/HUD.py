import pygame
from src.GAME.gamelogic import *
import subprocess


class HUD:
    def __init__(self,windowsize:tuple[int,int])->None:
        self.img:pygame.Surface = pygame.image.load("./src/img/panel.png").convert_alpha()
        self.img = pygame.transform.scale(self.img,windowsize)
        self.margin:int = 15
        self.topmargin:int = windowsize[0]-2*7
        self.leftmargin:int = 25
        self.width:int = (windowsize[0]-2*self.margin) - self.leftmargin
        self.height:int = 190
        self.bluematerial:int = 0
        self.greenmaterial:int = 0
        self.yellowmaterial:int = 0
        self.time:int = 0 #half hours
        self.rtime:int = 0 #IDK It dummy
        self.battery:int = 100 #without %
        self.speed:int = 0 #0,1,2,3
        self.font = pygame.font.Font("src/font/PressStart2P-Regular.ttf", 13)
        self.fontcolor = (150, 150, 150)
        self.epress = False
        self.actlog = "2026.02.27 12.48.01" #It's 
        self.proc = None

        #Panels
        panelcolors = (50, 50, 50)
        #Main panel
        pygame.draw.rect(self.img, (70, 70, 70), pygame.Rect(self.leftmargin, self.topmargin, self.width, self.height), border_radius=5)
        #Material panel
        pygame.draw.rect(self.img, panelcolors, pygame.Rect(self.leftmargin+10, self.topmargin+5, 120, self.height-10), border_radius=5)
        #Time panel
        pygame.draw.rect(self.img, panelcolors, pygame.Rect(self.leftmargin+150, self.topmargin+5, self.width-245, 40), border_radius=5)
        #Time hr
        pygame.draw.rect(self.img, (40,40,40), pygame.Rect((self.leftmargin+150)+self.width-490, self.topmargin+5, 5, 40), border_radius=5)
        #Battery panel
        pygame.draw.rect(self.img, panelcolors, pygame.Rect(self.width-50, self.topmargin+5, 70, self.height-10), border_radius=5)
        #Speed panel
        pygame.draw.rect(self.img, panelcolors, pygame.Rect(self.leftmargin+150, self.topmargin+70, self.width-245, 40), border_radius=5)
        #Speed hr
        pygame.draw.rect(self.img, (40,40,40), pygame.Rect((self.leftmargin+150)+self.width-490, self.topmargin+70, 5, 40), border_radius=5)

        #Ores
        oresimg = pygame.image.load("./src/img/ores.png").convert_alpha()
        oresrect = pygame.Rect(0,0, 64, 64)
        oreswidth:int = 64

        #Blue material drawing
        oresrect.x = oreswidth
        self.img.blit(oresimg, (self.leftmargin+20, self.topmargin+9), oresrect)

        #Green material drawing
        oresrect.x = int(oreswidth*3)
        self.img.blit(oresimg, (self.leftmargin+20, self.topmargin+60), oresrect)

        #Yellow material drawing
        oresrect.x = int(oreswidth*2)
        self.img.blit(oresimg, (self.leftmargin+20, self.topmargin+120), oresrect)


    def update(self, logicmodule:GameLogic)->None:
        self.time = logicmodule.simulationTime
        self.battery = logicmodule.rover.battery
        self.rtime = 100000-self.time

        if pygame.key.get_pressed()[pygame.K_e] and self.epress == False:
            self.proc = subprocess.Popen([
                "Dashboard.exe",
                self.actlog
                ])
        
        self.epress = pygame.key.get_pressed()[pygame.K_e]

    def show(self, screen:pygame.Surface)->None:
        # Aspect ratio is handled already so we dont need to check the height
        screen.blit(self.img,(0,0))
        fontcolor = self.fontcolor
        self.batteryimgs = [
            pygame.image.load(f"./src/img/battery/battery{i}.png").convert_alpha()
            for i in range(8)
        ]

        
        #Blue material drawing
        bluematerialtxt = self.font.render(str(self.bluematerial), True, fontcolor)
        screen.blit(bluematerialtxt, ((self.leftmargin+20)+70, (self.topmargin+10)+23))

        #Green material drawing
        greenmaterialtxt = self.font.render(str(self.greenmaterial), True, fontcolor)
        screen.blit(greenmaterialtxt, ((self.leftmargin+20)+70, (self.topmargin+60)+23))

        #Yellow material drawing
        yellowmaterialtxt = self.font.render(str(self.yellowmaterial), True, fontcolor)
        screen.blit(yellowmaterialtxt, ((self.leftmargin+20)+70, (self.topmargin+120)+23))

        #Time
        timetxt = self.font.render(f"Idő: {self.time//60:02d}:{self.time%60:02d}", True, fontcolor)
        screen.blit(timetxt, ((self.leftmargin+150)+10, (self.topmargin+5)+15))

        #Remaining
        #IT'S DUMMY
        rtimetxt = self.font.render(f"Hátralévő: {self.rtime//60:02d}:{self.time%60:02d}", True, fontcolor)
        screen.blit(rtimetxt, (((self.leftmargin+150)+self.width-490)+10, (self.topmargin+5)+15))

        #Battery      
        batteryimg = self.batteryimgs[int((self.battery/100)*7)]
        batteryimg = pygame.transform.scale(batteryimg,(60, self.height-50))
        screen.blit(batteryimg, (self.width-45, self.topmargin+10), pygame.Rect(0,0, 60, self.height-50))
        batterytxt = self.font.render(f"{self.battery}%", True, fontcolor)
        screen.blit(batterytxt, (self.width-40, (self.topmargin+10)+150))

        #Speed
        speed = ["Áll", "Lassú", "Normál", "Gyors"]
        speedtxt = self.font.render(f"{speed[self.speed]}", True, fontcolor)
        screen.blit(speedtxt, ((self.leftmargin+150)+10, (self.topmargin+70)+15))

        #Mining
        #IT'S DUMMY
        mingingtxt = self.font.render("--", True, fontcolor)
        screen.blit(mingingtxt, (((self.leftmargin+150)+self.width-490)+10, (self.topmargin+70)+15))

        #Dashboard text
        Dashtxt = pygame.font.Font("src/font/PressStart2P-Regular.ttf", 10).render("A dashboard megnyitásához nyomd meg az E-t!", False, (255, 255, 255))
        screen.blit(Dashtxt, ((self.leftmargin+150)-10, (self.topmargin+70)+70))

        if self.proc != None and self.proc.poll() == None:
            Dashstxt = pygame.font.Font("src/font/PressStart2P-Regular.ttf", 8).render("Már fut! Csak eltarthat egy ideig az índítás.", True, (200, 200, 200))
            screen.blit(Dashstxt, ((self.leftmargin+150)-10, (self.topmargin+70)+90))
