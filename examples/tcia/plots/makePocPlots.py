#from cycler import cycler
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
import csv
import os
import re
import sys

color = ['#000099', '#ff6600', '#3333ff',
         '#cc0000',
         'purple', 'pink', '#000000']

patterns = [ "/" ,"+" , "x", "o", "O", ".", "*",  "\\" , "|" , "-" ]
linestyles = [ ':sk', '--ok' , ':sk' ,'--ok', '-sk',  '-^k' ]

def main():

    if len(sys.argv) > 1:
        file_stacked  = str(sys.argv[1])
        file_metadata = str(sys.argv[2])
    else:
        file_stacked  = 'poc_results.txt'
        file_metadata = 'poc_scaleresults.txt'

    print "Files used: "
    print file_stacked
    print file_metadata

    form = 'pdf' # change to pdf for the final form

    print "Plotting Images per second... "
    # In this case, we plot only meta_load and spl_fileops
    plotcols = [0,1]
    createBars (file_stacked, form, plotcols, 'fig_ips.' + form)

    print "Plotting metadata scaling... "
    # This case uses a different file
    createMetadata(file_metadata, form, 'fig_metadata.' + form, 1)

def isfloat(value):
  try:
    float(value)
    return True
  except ValueError:
    return False

def createBars(filename, form, plotcols, outfilename):

    # Read the file
    configs = []
    with open(filename) as f:
        for line in f:
            if 'labels' in locals():
                all_data = line.rstrip('\n').split(',')
                configs.append(all_data[0]) # to deal with blank
                if 'data' in locals():
                    data = np.vstack([data, all_data[1::]])
                else:
                    data = all_data[1::]

            else:
                labels = line.rstrip('\n').split(',')[1::] # to deal with blank

    data = np.array(data).astype(np.float)
    data = data[:, plotcols]
    labels = np.array(labels)
    labels = labels[plotcols]

    # print labels
    # print configs
    # print data

    ind = np.arange(data.shape[1])    # the x locations for the groups
    width = 0.4       # the width of the bars: can also be len(x) sequence

    bar_width = 0.3
    n_groups = data.shape[0]
    index = np.arange(n_groups)
    opacity = 0.7

    fig, ax0 = plt.subplots(nrows=1)

    color_local = ['#cc0000','#3333ff']
    for i in range(data.shape[1]):

        bar = plt.bar(index + i*bar_width, data[:,i], bar_width,
                alpha=opacity,
                color=color_local[i],
                # hatch=patterns[i],
                # yerr=yy[:,1+2*i+1],
                # error_kw=error_config,
                label=labels[i]
                )
        # ax0.text(bar.get_x() + bar.get_width()/2,
        #         5, "cagon", ha='center', va='bottom')

    plt.ylabel('Images per second')
    # plt.title('PICK THE RIGHT TITLE')
    ax0.set_xticks(index + bar_width/2)
    ax0.set_xticklabels(configs, rotation=0, fontsize=10)

    rects = ax0.patches

    # Now make some labels
    labels = []
    labels.append("0")
    labels.append("0")
    labels.append("0")
    labels.append("{:2.2f}".format(data[0,1]/data[0,0])+"x")
    labels.append("{:2.2f}".format(data[1,1]/data[1,0])+"x")
    labels.append("{:2.2f}".format(data[2,1]/data[2,0])+"x")

    for rect, label in zip(rects, labels):
        if label == "0":
            continue
        height = rect.get_height()
        ax0.text(rect.get_x() + rect.get_width()/2,
                 height + 5, label, ha='center', va='bottom')

    plt.legend(
        loc='center', bbox_to_anchor=(0.35, 0.95),
        fancybox=True, shadow=True, ncol=4, fontsize=10)

    plt.savefig(outfilename, format=form, bbox_inches='tight')
    plt.close()

    return

def createMetadata(filename, form, outputfile, metadata_col):

    configs = []
    with open(filename) as f:
        for line in f:
            if 'labels' in locals():
                all_data = line.rstrip('\n').split(',')
                configs.append(all_data[0]) # to deal with blank
                if 'data' in locals():
                    data = np.vstack([data, all_data[1::]])
                else:
                    data = all_data[1::]

            else:
                labels = line.rstrip('\n').split(',')[1::] # to deal with blank

    data = np.array(data).astype(np.float)

    # print labels
    # print configs
    #print data

    plt.rc('lines', linewidth=3)
    fig, ax0 = plt.subplots(nrows=1)

    #ax0.spines['top'].set_visible(False)
    #ax0.spines['bottom'].set_visible(False)
    #ax0.spines['right'].set_visible(False)
    #ax0.spines['left'].set_visible(False)

    # Ensure that the axis ticks only show up on the bottom and left of the plot.
    # Ticks on the right and top of the plot are generally unnecessary.
    ax0.get_xaxis().tick_bottom()
    ax0.get_yaxis().tick_left()

    # Remove the tick marks; they are unnecessary with the tick lines we just
    # plotted.
    plt.tick_params(axis='both', which='both', bottom='off', top='off',
                    labelbottom='on', left='off', right='off', labelleft='on')

    i = 0
    #print 'WARNING, RANDOM DATA ADDED FOR TESTING, REMOVE WITH FINAL DATA'
    for row in data:
        # COMMENT THE NEXT LINE WITH FINAL DATA
        # row = row + np.ones(row.shape[0]) + i + np.random.rand(row.shape[0])
        ax0.semilogy(row, linestyles[i], label=configs[i],
              color=color[i])
        i = i+1

    plt.legend(loc="best", ncol=1, fancybox=True)
    ax0.set_xticklabels(labels)
    ax0.set_ylabel('Execution Time (ms)')
    plt.xlabel('Number of Images in the Database')

    ax1 = ax0.twinx()
    ax1.plot( [1.29, 1.55, 1.67], '-^k', color=color[0], linewidth=0.6, label="something")
    ax1.plot( [5.79, 31.55, 58.67], '-^k', color=color[1], linewidth=0.6, label="something")
    ax1.set_ylabel('CPU Utilization(%)')
    ax1.set_yticks([0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100])

    plt.xticks( list(xrange(data.shape[1])), rotation=0)

    # plt.ylabel('Execution Time (ms)')
    # plt.title('Metadata Time')

    #plt.ticklabel_format(style='sci', axis='y', scilimits=(0,0))


    plt.savefig(outputfile, format=form, bbox_inches='tight')
    plt.close()

    return

if __name__ == '__main__':
    main()
