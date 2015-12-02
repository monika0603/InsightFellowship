from bs4 import BeautifulSoup
import urllib2
import io
from ROOT import TCanvas, TGraph, gStyle
from ROOT import gROOT
from array import array
import datetime
import glob
from dominate import document
from dominate.tags import *

def extractDataTables():
    wiki = "https://en.wikipedia.org/wiki/List_of_earthquakes_in_California"
    header = {'User-Agent': 'Mozilla/5.0'} # Needed to prevent 403 error on Wikipedia
    req = urllib2.Request(wiki,headers=header)
    page = urllib2.urlopen(req)
    soup = BeautifulSoup(page, "html.parser")

    table = soup.find("table", {"class" : "wikitable sortable" })

    date = ""
    PTZ = ""
    area = ""
    magnitude = ""
    misc1 = ""
    misc2 = ""
    deaths = ""
    coordinates = ""
    comments = ""
    misc3 = ""

    f = io.open('output.csv', 'w', encoding='utf8')

    for row in table.findAll("tr"):
        cells = row.findAll("td")
        #For each "tr", assign each "td" to a variable.
        if len(cells) == 10:
            date = cells[0].findAll(text=True)
            PTZ = cells[1].find(text=True)
            area = cells[2].find(text=True)
            magnitude = cells[3].findAll(text=True)
            misc1 = cells[4].find(text=True)
            misc2 = cells[5].find(text=True)
            deaths = cells[6].find(text=True)
            coordinates = cells[7].find(text=True)
            comments = cells[8].find(text=True)
            misc3 = cells[9].find(text=True)

            #date is a list of list, so iterating through the top level first
        for x in range(len(date)):
            #For each list, split the string
            earthquake_date = date[x].split()
            eartquake_magnitude = magnitude[x].split()
            eartquake_magnitude.append("\n")
            year = [i.split('-')[0] for i in date]
            #For each item in the split list...
            for i in range(len(earthquake_date)):
                write_to_file = year + eartquake_magnitude
                #print write_to_file
                f.write(','.join(write_to_file))

    f.close()

    xVals = []
    yVals = []
    yVals1 = []
    yVals2 = []
    #Tayloring field values to pass to TGraph as floats.
    with open('output.csv', 'r') as f:
        for line in f:
            line = line.strip()
            try:
                k, v = line.split(',',1)
                xVals.append(k)
                yVals.append(v)
            except ValueError:
                print('Ignoring: malformed line: "{}"'.format(line))

    for hh in range(len(yVals)):
        yVals1.append((yVals[hh].replace(",","")))
        if(len(yVals[hh]) > 4):
            aa = yVals[hh][:3]
            yVals1.pop(hh)
            yVals1.insert(hh, aa)

    xVals1 = map(float, xVals)
    yVals2 = map(float, yVals1)

    return (xVals1, yVals2)


def plot(rVals1, rVals2):
    xArray = array('d', rVals1)
    yArray = array('d', rVals2)

    gStyle.SetOptStat(0);
    gStyle.SetOptFit(0);
    gStyle.SetPalette(1);
    gStyle.SetFrameFillColor(0);
    gStyle.SetCanvasColor(0);
    gStyle.SetPadBorderSize(0);
    gStyle.SetCanvasBorderSize(0);
    gStyle.SetPadColor(0);
    gStyle.SetPadBorderMode(0);
    gStyle.SetPadTickX(1);
    gStyle.SetPadTickY(1);
    gStyle.SetLegendBorderSize(0);
    gStyle.SetErrorX(0.5);
    gStyle.SetTextFont(42);
    gStyle.SetLabelFont(42,"x");
    gStyle.SetLabelFont(42,"y");
    gStyle.SetTitleFont(42,"x");
    gStyle.SetTitleFont(42,"y");
    gStyle.SetEndErrorSize(1);

    # Create a new canvas, and customize it.
    c1 = TCanvas( 'c1', 'Earthquake history in California', 200, 10, 900, 700 )
    c1.SetFillColor(0)
    c1.GetFrame().SetFillColor(21)
    c1.GetFrame().SetBorderSize( 6 )
    c1.GetFrame().SetBorderMode( -1 )
    c1.SetLeftMargin(0.1439791);
    c1.SetRightMargin(0.03534031);
    c1.SetTopMargin(0.07777778);
    c1.SetBottomMargin(0.145098);

    gr = TGraph( len(xArray), xArray, yArray)
    gr.SetMarkerStyle(20)
    gr.SetTitle('')
    gr.SetMaximum(9.5)
    gr.SetMinimum(4.5)
    gr.GetXaxis().SetTitle('Year #rightarrow')
    gr.GetXaxis().SetTitleSize(0.06);
    gr.GetXaxis().SetLabelFont(42);
    gr.GetXaxis().SetLabelSize(0.055);
    gr.GetXaxis().SetTitleOffset(1.09);
    gr.GetYaxis().SetTitle('Earthquake magnitude')
    gr.GetYaxis().SetTitleSize(0.06);
    gr.GetYaxis().SetLabelFont(42);
    gr.GetYaxis().SetLabelSize(0.055);
    gr.GetYaxis().SetTitleOffset(1.14);
    
    gr.Draw( 'AC*' )
    c1.SaveAs("plot.png")

    photos = glob.glob("plot.png")
    
    with document(title="Earthquake history in California") as doc:
        h1('Earthquake history in California')
        for path in photos:
            div(img(src=path), _class='photo')
        attr(cls='body')
        p('This plot shows seismic activity as a function of time in California. Average frequency of 6.5 magnitude earthquake on Ricter scale has gone up in the recent years.')

    with open('California.html', 'w') as ff:
        ff.write(doc.render())

if __name__ == '__main__':
    rVals1, rVals2 = extractDataTables()
    plot(rVals1, rVals2)




