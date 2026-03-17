from customtkinter import *
from CTkListbox import *
import matplotlib.pyplot as plt
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.colors import ListedColormap
import csv

class FullGraphWindow:
    def __init__(self, main, title, xdata, ydata, ylabel, xlabel, mode="line", ticks=None, ticklabels=None, alldata=None):
        self.win = CTkToplevel(main)
        self.win.title("Dashboard - " + title)
        self.win.geometry("900x600")
        self.win.resizable(True, True)
        self.win.iconbitmap("src/img/dashboardicon.ico")

        self.fg, self.ax = plt.subplots(figsize=(4, 3), dpi=100)

        self.canvas = FigureCanvasTkAgg(self.fg, master=self.win)
        self.canvas.get_tk_widget().pack(fill="both", expand=True)

        self.fg.patch.set_facecolor("#262626")
        self.ax.set_facecolor("#262626")
        self.ax.tick_params(colors="white")
        self.ax.xaxis.label.set_color("white")
        self.ax.yaxis.label.set_color("white")
        self.ax.spines["bottom"].set_color("white")
        self.ax.spines["left"].set_color("white")
        self.ax.spines["top"].set_color("white")
        self.ax.spines["right"].set_color("white")

        self.mode = mode
        self.xdata = xdata
        self.ydata = ydata
        self.ylabel = ylabel
        self.xlabel = xlabel
        self.ticks = ticks
        self.ticklabels = ticklabels
        self.alldata = alldata

        if mode == "coords":
            self.ax.grid(True)
            self.ax.set_xlim(0, 50)
            self.ax.set_ylim(0, 50)
            mars_map = []

            with open("src/PATHFINDER/mars_map_50x50.csv", "r") as f:
                reader = csv.reader(f)
                for row in reader:
                    mars_map.append(row)

            grid = []

            self.start_x = 0
            self.start_y = 0

            for y in range(50):
                grid_row = []
                for x in range(50):
                    tile = mars_map[y][x]

                    if tile == "#":
                        grid_row.append(1)

                    elif tile == "B":
                        grid_row.append(2)

                    elif tile == "Y":
                        grid_row.append(3)

                    elif tile == "G":
                        grid_row.append(4)

                    elif tile == "S":

                        grid_row.append(5)

                        self.start_x = x
                        self.start_y = y

                    else:
                        grid_row.append(0)

                grid.append(grid_row)

            cmap = ListedColormap([
                "#2e3237",
                "gray",
                "blue",
                "yellow",
                "green",
                "lightgreen"
            ])

            self.ax.imshow(
                grid,
                cmap=cmap,
                origin="upper",
                extent=(0,50,50,0)
            )

            if alldata != None:

                self.traveled_coords = self.ax.scatter(
                    self.start_x,
                    self.start_y,
                    color="white",
                    alpha=0.3,
                    s=40
                )

                self.traveled_coords_line = self.ax.plot(
                        self.start_x,
                        self.start_y,
                        color="white",
                        alpha=0.3
                    )
                
            self.rover_coords = self.ax.scatter(self.start_x, self.start_y, color="red", s=40)

            self.rover_coordstxt = self.ax.annotate("Rover", (self.start_x, self.start_y))
            self.rover_coordstxt.set_fontweight("bold")
            self.rover_coordstxt.set_color("black")

            self.ax.scatter(self.start_x, self.start_y, color="lightgreen", s=40)
            starttxt = self.ax.annotate("Kiinduló Pont", (self.start_x, self.start_y))
            starttxt.set_fontweight("bold")
            starttxt.set_color("lightgreen")

            self.ax.set_aspect("auto")
            self.ax.invert_yaxis()

        self.update_graph()

    def update_graph(self):
        if self.mode != "coords":
            self.ax.clear()
            if self.mode == "line":
                self.ax.plot(self.xdata, self.ydata, marker="o")
            elif self.mode == "bar":
                self.ax.bar(self.xdata, self.ydata, width=20, linewidth=1, color="lightgreen")
            self.ax.set_ylabel(self.ylabel, color="white")
            self.ax.set_xlabel(self.xlabel, color="white")

            if self.ticks is not None and self.ticklabels is not None:
                self.ax.set_yticks(self.ticks)
                self.ax.set_yticklabels(self.ticklabels)

            self.ax.set_xticks(self.xdata)

            labels = []

            for i in range(len(self.xdata)):
                if len(self.xdata) >= 32:
                    if i % 4  == 0:
                        labels.append(f"{int(self.xdata[i])//60}:{int(self.xdata[i])%60:02d}")                
                    else:
                        labels.append("")
                else:
                    labels.append(f"{int(self.xdata[i])//60}:{int(self.xdata[i])%60:02d}")

            
            self.ax.set_xticklabels(labels)
            self.ax.tick_params("x", rotation=90)
            self.ax.grid(True, linestyle="--", linewidth=0.5)
            self.ax.set_facecolor("#262626")
            self.fg.patch.set_facecolor("#262626")
            self.canvas.draw_idle()
        
        else:
            if self.alldata != None:
                alldatax = [self.start_x]
                alldatay = [self.start_y]
                alldata = [(self.start_x, self.start_y)]
                
                for i in self.alldata:
                    alldata.append(i)

                
                for i in range(len(alldata)):
                    self.traveled_coords = self.ax.scatter(
                        alldata[i][0],
                        alldata[i][1],
                        color="white",
                        alpha=0.3,
                        s=40
                    )

                    alldatax.append(
                        alldata[i][0]
                    )

                    alldatay.append(
                        alldata[i][1]
                    )

                self.traveled_coords_line = self.ax.plot(
                        alldatax,
                        alldatay,
                        color="white",
                        alpha=0.3
                    )
                
            self.ax.set_ylabel(self.ylabel, color="white")
            self.ax.set_xlabel(self.xlabel, color="white")
            self.rover_coords.set_offsets([self.xdata, self.ydata])
            self.rover_coordstxt.set_position([self.xdata, self.ydata])
            self.canvas.draw_idle()