import pygame


class HUD:
    def __init__(self,windowsize:tuple[int,int])->None:
        self.img:pygame.Surface = pygame.image.load("./src/img/panel.png")
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
        self.rtime:int = 60 #IDK It dummy
        self.battery:int = 100 #without %

    def show(self, screen:pygame.Surface)->None:
        # Aspect ratio is handled already so we dont need to check the height
        screen.blit(self.img,(0,0))
        font = pygame.font.Font(None, 36)
        oresimg = pygame.image.load("./src/img/ores.png").convert_alpha()
        oresrect = pygame.Rect(0,0, 64, 64)
        oreswidth:int = 64
        fontcolor = (150, 150, 150)
        panelcolors = (50, 50, 50)
        batteryimg = pygame.image.load("./src/img/battery/battery7.png").convert_alpha()

        pygame.draw.rect(screen, (70, 70, 70), pygame.Rect(self.leftmargin, self.topmargin, self.width, self.height), border_radius=5)
        pygame.draw.rect(screen, panelcolors, pygame.Rect(self.leftmargin+10, self.topmargin+5, 120, self.height-10), border_radius=5)
        
        #Blue material drawing
        oresrect.x = oreswidth
        screen.blit(oresimg, (self.leftmargin+20, self.topmargin+9), oresrect)
        bluematerialtxt = font.render(str(self.bluematerial), True, fontcolor)
        screen.blit(bluematerialtxt, ((self.leftmargin+20)+65, (self.topmargin+10)+15))

        #Green material drawing
        oresrect.x = int(oreswidth*3)
        screen.blit(oresimg, (self.leftmargin+20, self.topmargin+60), oresrect)
        greenmaterialtxt = font.render(str(self.greenmaterial), True, fontcolor)
        screen.blit(greenmaterialtxt, ((self.leftmargin+20)+65, (self.topmargin+60)+15))

        #Yellow material drawing
        oresrect.x = int(oreswidth*2)
        screen.blit(oresimg, (self.leftmargin+20, self.topmargin+120), oresrect)
        yellowmaterialtxt = font.render(str(self.yellowmaterial), True, fontcolor)
        screen.blit(yellowmaterialtxt, ((self.leftmargin+20)+65, (self.topmargin+120)+15))

        #Time
        pygame.draw.rect(screen, panelcolors, pygame.Rect(self.leftmargin+150, self.topmargin+5, self.width-245, 40), border_radius=5)

        timetxt = font.render(f"{self.time//60:02d}:{self.time%60:02d}", True, fontcolor)
        screen.blit(timetxt, ((self.leftmargin+150)+10, (self.topmargin+5)+10))

        #Remaining
        pygame.draw.rect(screen, (40,40,40), pygame.Rect((self.leftmargin+150)+self.width-490, self.topmargin+5, 5, 40), border_radius=5)

        #IT'S DUMMY
        rtimetxt = font.render(f"Hátralévő: {self.rtime//60:02d}:{self.time%60:02d}", True, fontcolor)
        screen.blit(rtimetxt, (((self.leftmargin+150)+self.width-490)+10, (self.topmargin+5)+10))

        #Battery
        pygame.draw.rect(screen, panelcolors, pygame.Rect(self.width-50, self.topmargin+5, 70, self.height-10), border_radius=5)
        
        batteryimg = pygame.image.load(f"./src/img/battery/battery{int((self.battery / 100) * 7)}.png").convert_alpha()
        batteryimg = pygame.transform.scale(batteryimg,(60, self.height-50))
        screen.blit(batteryimg, (self.width-45, self.topmargin+10), pygame.Rect(0,0, 60, self.height-50))
        batterytxt = font.render(f"{self.battery}%", True, fontcolor)
        screen.blit(batterytxt, (self.width-45, (self.topmargin+10)+145))