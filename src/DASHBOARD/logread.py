import os

def logread(logfile, logsize:int, logline:int, time:list, battery:list, positionY:int, positionX:int, speed:list, distance:list, materialB:int, materialY:int, materialG:int, events:list):
    if logfile != None and len(logfile) > 0 and logsize < os.path.getsize(logfile):
        with open(logfile, "r") as fullog:
            lines = fullog.readlines()
            materialch:str = ""
            speedch:str = ""

            for i in range(logline, len(lines)):

                fullline:list = lines[i].strip().split(";")

                positionX = int(fullline[0].split(",")[0])
                positionY = int(fullline[0].split(",")[1])
                battery.append(int(fullline[1]))
                speed.append(int(fullline[2]))
                distance.append(int(fullline[2]))

                if len(fullline) >= 4:

                    if fullline[3].lower() == "y":
                        materialY += 1
                        materialch = "| ⛏ Sárga ásvány"

                    elif fullline[3].lower() == "b":
                        materialB += 1
                        materialch = "| ⛏ Kék ásvány"

                    elif fullline[3].lower() == "g":
                        materialG += 1
                        materialch = "| ⛏ Zöld ásvány"

                if len(time) == 0:
                    time.append(0)
                else:
                    time.append(time[len(time) - 1] + 30)

                if int(fullline[2]) == 3:
                    speedch = "Gyors"
                elif int(fullline[2]) == 2:
                    speedch = "Normál"
                elif int(fullline[2]) == 1:
                    speedch = "Lassú"
                elif int(fullline[2]) == 0:
                    speedch = "Áll"

                events.append(
                    f"[{int(time[len(time)-1])//60}:{int(time[len(time)-1])%60:02d}] ⚡{battery[len(battery)-1]}% | 🚀 {speedch} {materialch}"
                )
                materialch = ""

            logline = len(lines)
            logsize = os.path.getsize(logfile)

    return logsize, logline, time, battery, positionY, positionX, speed, distance, materialB, materialY, materialG, events
