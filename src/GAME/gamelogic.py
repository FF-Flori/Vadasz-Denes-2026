import pygame
from math import ceil
from random import randint
from src.GAME.rover import *
from datetime import datetime
from src.PATHFINDER.build.pathfinder import *

class GameLogic:
    def __init__(self, scrwidth, scrheight) -> None:
        self.width = scrwidth
        self.height = scrheight
        self.speed:float = 20
        self.viewed:list[float] = [0,0]
        self.oreSrc:pygame.Surface = pygame.image.load("./src/img/ores.png").convert_alpha()
        self.oreImgs:pygame.Surface = self.oreSrc
        self.overlay:pygame.Surface = pygame.Surface((scrwidth,scrheight)).convert_alpha()
        self.overlay.fill((0,0,0,64))
        self.orewidth:float = 64
        self.map:list[list[str]] = []
        with open("./mars_map_50x50.csv") as file:
            for row in file.readlines():
                self.map.append(row.strip().split(','))
        self.viewedWidth:float= self.width/self.orewidth
        self.rover:Rover = Rover(self.map)
        self.rover.setGear(1)

        self.zoom:float = 1
        self.createBG()
        self.scaledBG:pygame.Surface = self.background
        
        self.simulationTime:int = 0
        self.setTime:int = 0
        self.mined:str = ''
        self.blueorenum:int = 0
        self.yelloworenum:int = 0
        self.greenorenum:int = 0
        self.framesToTimeInc:int = 1
        self.isday:bool = True

        self.impossible:bool = False
        self.impossibleOverlay:pygame.Surface = pygame.Surface((scrwidth,scrheight))
        impofont:pygame.font.Font = pygame.font.Font("./src/font/PressStart2P-Regular.ttf",18)
        text:pygame.Surface = impofont.render("Ennyi idővel lehetetlen bármennyi",True,'white')
        self.impossibleOverlay.blit(text,((scrwidth-text.get_width())//2,20))
        text = impofont.render("ércet összegyűjteni. Próbálj",True,'white')
        self.impossibleOverlay.blit(text,((scrwidth-text.get_width())//2,60))
        text = impofont.render("meg megadni több időt!",True,'white')
        self.impossibleOverlay.blit(text,((scrwidth-text.get_width())//2,100))

        now = datetime.now()
        self.logname:str = '.'.join(str(datetime.date(now)).split('-'))+' '+'.'.join(str(datetime.time(now)).split('.')[0].split(':'))
        with open("./log/"+self.logname+".log","w",encoding="utf-8") as file:
            file.write("<pozició (x,y)>;<akkumlátor töltöttség (%)>;<sebbesség és megtett távolság>;<gyűjtött anyagok>")

        self.paused:bool = False
        self.pausedonlast:bool = False
        self.scale()
        # Starts at 0 0, so its just the coords of the rover
        self.moveCamera(self.rover.pos[0]-self.viewedWidth//2,self.rover.pos[1]-self.viewedWidth//2)

    def getDisplacement(self,value)->tuple[int,int]:
        if value == Instruction.UP_LEFT:
            return (-1,-1)
        elif value == Instruction.UP:
            return (0,-1)
        elif value == Instruction.UP_RIGHT:
            return (1,-1)
        elif value == Instruction.RIGHT:
            return (1,0)
        elif value == Instruction.DOWN_RIGHT:
            return (1,1)
        elif value == Instruction.DOWN:
            return (0,1)
        elif value == Instruction.DOWN_LEFT:
            return (-1,1)
        elif value == Instruction.LEFT:
            return (-1,0)
        else:
            return (0,0)
    def SetTimeValue(self, simtime:int)->None:
        self.setTime = simtime
        Pathfinder.create(simtime//30,"./mars_map_50x50.csv")
        pf = Pathfinder.get_instance()
        self.route = pf.calculate()
        if len(self.route) == 0:
            self.impossible = True
        Pathfinder.destroy()
        self.posinRoute:int = 0
    def writeToLog(self)->None:
        with open("./log/"+self.logname+".log","a") as file:
            file.write("\n"+str(int(self.rover.pos[0]))+","+str(int(self.rover.pos[1]))+";"+str(self.rover.battery)+";"+str(self.rover.gear)+";"+self.mined)
    def createBG(self)->None:
        source:pygame.Surface = pygame.image.load("./src/img/bg.png").convert_alpha()
        tilesize:int = 64
        self.background:pygame.Surface = pygame.Surface((tilesize*len(self.map),tilesize*len(self.map[0])))

        srcRect:pygame.Rect = pygame.Rect(0,0,tilesize,tilesize)
        for x in range(len(self.map)):
            for y in range(len(self.map[0])):
                randnum:int = randint(0,99)
                # 80% for the topleft
                # 10% for the topright
                # 7% for the bottomleft
                # 3% for the bottomright
                if randnum > -1 and randnum < 80:
                    srcRect.x = 0
                    srcRect.y = 0
                elif randnum < 90:
                    srcRect.x = tilesize
                    srcRect.y = 0
                elif randnum < 97:
                    srcRect.x = 0
                    srcRect.y = tilesize
                else:
                    srcRect.x = tilesize
                    srcRect.y = tilesize
                self.background.blit(source,(x*tilesize,y*tilesize),srcRect)

    # Posx and Posy store where the ore is in the map
    def drawOre(self, oreType:str, posx:float, posy:float, screen:pygame.Surface):
        oreRect = pygame.Rect(0,0,self.orewidth,self.orewidth)
        if oreType == 'Y':
            oreRect.x = int(self.orewidth*2)
            screen.blit(self.oreImgs,(posx,posy), oreRect)
        if oreType == 'G':
            oreRect.x = int(self.orewidth*3)
            screen.blit(self.oreImgs,(posx,posy), oreRect)
        if oreType == 'B':
            oreRect.x = int(self.orewidth)
            screen.blit(self.oreImgs,(posx,posy), oreRect)
        if oreType == '#':
            oreRect.x = 0
            screen.blit(self.oreImgs,(posx,posy), oreRect)
    def scale(self):
        if self.zoom > 2:
            self.zoom = 2
        if self.zoom < 0.3:
            self.zoom = 0.3
        center:list[float] = [self.viewed[0]+self.viewedWidth/2,self.viewed[1]+self.viewedWidth/2]
        self.orewidth = self.oreSrc.get_height()*self.zoom
        neworewidth = self.oreSrc.get_height()*self.zoom
        if ceil(self.width//neworewidth) >= len(self.map):
            self.zoom += 0.1
            return
        self.viewedWidth = self.width/self.orewidth
        self.oreImgs = pygame.transform.scale(self.oreSrc,(self.oreSrc.get_width()*self.zoom,self.oreSrc.get_height()*self.zoom))
        self.rover.scaled = pygame.transform.scale(self.rover.sprite,(self.rover.sprite.get_width()*self.zoom,self.rover.sprite.get_height()*self.zoom))
        self.rover.scaledwidth = self.rover.spritewidth*self.zoom
        self.moveCamera(center[0]-self.viewedWidth/2-self.viewed[0],center[1]-self.viewedWidth/2-self.viewed[1])
        self.scaledBG = pygame.transform.scale(self.background,(self.background.get_width()*self.zoom,self.background.get_height()*self.zoom))
    def moveCamera(self,dispX:float,dispY:float)->None:
        self.viewed[0] += dispX
        self.viewed[1] += dispY
        if dispX != 0:
            if self.viewed[0] < 0:
                self.viewed[0] = 0
            elif self.viewed[0]+self.viewedWidth > len(self.map[0]):
                self.viewed[0] = len(self.map[0])-self.viewedWidth
        if dispY != 0:
            if self.viewed[1] < 0:
                self.viewed[1] = 0
            elif self.viewed[1]+self.viewedWidth > len(self.map):
                self.viewed[1] = len(self.map)-self.viewedWidth
    def showPath(self,screen:pygame.Surface)->None:
        bounds:tuple[float,float] = (-self.orewidth,self.viewedWidth*self.orewidth+self.orewidth)
        lastpos:list[float] = self.rover.pos
        pos:list[int] = [0,0]
        speed:int = self.rover.gear
        loopstart = self.posinRoute-1
        if loopstart < 0:
            loopstart = 0
        for i in range(self.posinRoute-1,len(self.route)):
            value = self.route[i]
            if int(value) < 8:
                if speed == 0:
                    continue
                displacement = [*self.getDisplacement(value)]
                if displacement[0] < 0:
                    pos[0] = ceil(lastpos[0]) + displacement[0]
                else:
                    pos[0] = int(lastpos[0]) + displacement[0]

                if displacement[1] < 0:
                    pos[1] = ceil(lastpos[1]) + displacement[1]
                else:
                    pos[1] = int(lastpos[1]) + displacement[1]

            elif int(value) < 12:
                if value == Instruction.SET_SPEED_0:
                    speed = 0
                else:
                    speed = 1
                continue
            else:
                continue
            start:tuple[int,int] = (int((lastpos[0]-self.viewed[0])*self.orewidth),int((lastpos[1]-self.viewed[1])*self.orewidth))
            start = (int(start[0]+self.orewidth/2),int(start[1]+self.orewidth/2))
            end:tuple[int,int] = (int((pos[0]-self.viewed[0])*self.orewidth),int((pos[1]-self.viewed[1])*self.orewidth))
            end = (int(end[0]+self.orewidth/2),int(end[1]+self.orewidth/2))
            if (start[0] > bounds[0] and start[0] < bounds[1]) and \
               (start[1] > bounds[0] and start[1] < bounds[1]) and \
               (end[0] > bounds[0] and end[0] < bounds[1]) and \
               (end[1] > bounds[0] and end[1] < bounds[1]):
                   pygame.draw.line(screen,(255,255,0,128),start,end,4)

            lastpos = [*pos]


    def traversePath(self)->None:
        # Checking instructions
        self.mined = ''
        isday = (self.simulationTime//30)%48<32
        moved:bool = False
        while not moved and self.posinRoute < len(self.route):
            print(self.route[self.posinRoute])
            intvalue:int = int(self.route[self.posinRoute])
            value = self.route[self.posinRoute]
            if intvalue >= 0 and intvalue < 8:
                self.rover.moveTo(*self.getDisplacement(value))
                if self.rover.gear > 0:
                    self.rover.battery -= 2*self.rover.gear
                else:
                    self.rover.battery -=1
                moved = True
            elif intvalue < 12:
                if value == Instruction.SET_SPEED_1:
                    self.rover.setGear(1)
                    self.framesToTimeInc = 1
                elif value == Instruction.SET_SPEED_2:
                    self.rover.setGear(2)
                    self.framesToTimeInc = 2
                elif value == Instruction.SET_SPEED_3:
                    self.rover.setGear(3)
                    self.framesToTimeInc = 3
                elif value == Instruction.SET_SPEED_0:
                    self.rover.setGear(0)
                    self.framesToTimeInc = 1
                else:
                    print("Error")
                moved = False
            elif value == Instruction.MINE:
                pos:list[int] = [int(self.rover.pos[0]),int(self.rover.pos[1])]
                assert(self.map[pos[1]][pos[0]] != ".")
                assert(self.map[pos[1]][pos[0]] != "#")
                self.mined = self.map[pos[1]][pos[0]]
                if self.mined == 'Y':
                    self.yelloworenum += 1
                elif self.mined == 'G':
                    self.greenorenum += 1
                else:
                    self.blueorenum += 1
                self.map[pos[1]][pos[0]] = "."
                self.rover.startIdling()
                self.framesToTimeInc = 1
                moved = True
                self.rover.battery-=2
            else:
                moved = False
            self.posinRoute += 1

        # Changing time

        self.framesToTimeInc -= 1
        if self.framesToTimeInc < 1:
            if isday:
                self.rover.battery += 10
            self.framesToTimeInc = self.rover.gear
            self.simulationTime += 30
        if self.rover.battery > 100:
            self.rover.battery = 100
        elif self.rover.battery < 0:
            print("Why the fuck")
            self.rover.battery = 0

        self.writeToLog()
        self.isday = (self.simulationTime//30)%48<32
    # deltaTime is in miliseconds
    def Update(self,deltaTime:float,screen:pygame.Surface) -> None:
        # Input stuff
        keys = pygame.key.get_pressed()
        xdisp:float = 0
        ydisp:float = 0
        if keys[pygame.K_a] or keys[pygame.K_LEFT]:
            xdisp -= self.speed/self.zoom/1000*deltaTime
        if keys[pygame.K_w] or keys[pygame.K_UP]:
            ydisp -= self.speed/self.zoom/1000*deltaTime
        if keys[pygame.K_s] or keys[pygame.K_DOWN]:
            ydisp += self.speed/self.zoom/1000*deltaTime
        if keys[pygame.K_d] or keys[pygame.K_RIGHT]:
            xdisp += self.speed/self.zoom/1000*deltaTime
        if keys[pygame.K_SPACE]:
            xdisp = self.rover.pos[0]-self.viewedWidth/2-self.viewed[0]
            ydisp = self.rover.pos[1]-self.viewedWidth/2-self.viewed[1]
        self.moveCamera(xdisp,ydisp)
        if (keys[pygame.K_p] or keys[pygame.K_ESCAPE]) and not self.pausedonlast:
            self.paused = not self.paused
            print(self.viewed,self.viewedWidth)
        self.pausedonlast = keys[pygame.K_p] or keys[pygame.K_ESCAPE]

        # Logic stuff
        if not self.paused:
            self.rover.update(deltaTime)
            if self.setTime - self.simulationTime > 0 and not self.impossible:
                if self.rover.target[0] == -1 and self.rover.target[1] == -1 and self.posinRoute < len(self.route):
                    self.traversePath()

        #Drawing stuff
        screen.blit(self.scaledBG,(0,0),(self.viewed[0]*self.orewidth,self.viewed[1]*self.orewidth,self.width,self.height))
        self.showPath(screen)

        for y in range(int(self.viewed[1]),ceil(self.viewed[1]+self.viewedWidth)):
            for x in range(int(self.viewed[0]),ceil(self.viewed[0]+self.viewedWidth)):
                self.drawOre(self.map[y][x],(x-self.viewed[0])*self.orewidth,(y-self.viewed[1])*self.orewidth,screen)

        self.rover.draw(screen,self.orewidth,self.viewed,self.viewedWidth)
        if not self.isday:
            screen.blit(self.overlay,(0,0))
        if self.impossible:
            screen.blit(self.impossibleOverlay,(0,0))
