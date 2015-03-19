# script to summaryize processing information run by run

#print run_list
#print len(run_list)

#print run_list_irrad_thrnoise

import sys
if len(sys.argv) != 4:
        print 'Three arguments needed: output folder, first run, last run'
        sys.exit()
folder = sys.argv[1]
firstRun = sys.argv[2]
lastRun = sys.argv[3]
run_list = range(int(firstRun), int(lastRun)+1)
in_fileName = folder + 'analysis.log'
out_fileName = folder + 'analysis_summary.log'
in_file = open(in_fileName, 'r')
out_file = open(out_fileName, 'w')

for run in run_list:
        in_file.seek(0)

        print >>out_file, '*******************************************************'
        print >>out_file, 'RUN ', str(run)
        print >>out_file, '----------------------'

        for line in in_file:
                if str(run) in line:
                        print >>out_file, line[0: len(line)-1]

        #print >>out_file, '*******************************************************'
        print >>out_file, ''
print 'Reordered log file written to', out_fileName
