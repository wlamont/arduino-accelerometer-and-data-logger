# -*- coding: utf-8 -*-
"""
Created on Thu Jun 23 18:31:51 2016

@author: wlamont
"""

#import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def FFT(x, f_s):
	'''
	Calculates the FFT and returns the frequency and amplitudes	
	x = time series of signal
	f_s = sampling frequency	
	
	'''
	
	fft_x = np.fft.fft(x)
	n = len(fft_x)
	freq = np.fft.fftfreq(n, 1/f_s)
	half_n = np.ceil(n/2.0)
	fft_x_half = (2.0 / n) * fft_x[:half_n]
	freq_half = freq[:half_n]

	return freq_half, np.abs(fft_x_half)


#change the input directory below, leave the prefix 'r'
IDIR = r'/home/wlamont/Dropbox/PTY 769- Running app/Data/Accelerometer Data/P02'

# do not change sampling frequency
F_S = (1 / (30.0 / 1000.0)) # sampling frequency

# change the frequency window of interest
freq_window = (1, 2)

print '\n'
print 'Look at frequencies between ' + str(freq_window[0]) + ' and ' + str(freq_window[1]) 
print '\n'

for i in (2, 3, 4, 10): # put which ever trials you want seperated by commas
    
    if i <= 9:
        IFILE = 'LOGGER0' + str(i) + '.CSV' 
    else:
        IFILE = 'LOGGER' + str(i) + '.CSV' 
        
    print IFILE
#IFILE = 'LOGGER10.CSV'


    data    = pd.read_csv(IDIR + '/' +  IFILE, sep='\t', header=0, skiprows=[1])
    data['dt'] = data['millis '][:] - data['millis '][0] 
    
    N = 10
    
    fig = plt.figure(num=N)
    fig.subplots_adjust(hspace=0.8)
    plt.subplot(311)
    plt.title(IFILE + '\n' + data[' datetime '][0][2:-1])
    plt.plot(data['dt'], data[' x (G) '])
    plt.locator_params(axis='y', nbins=5)
    plt.xlabel('Time (ms)')
    plt.ylabel('X Acceleration [G]')
    plt.grid()            
    												
    plt.subplot(312)
    plt.plot(data['dt'], data[' y (G) '])
    plt.xlabel('Time (ms)')
    plt.ylabel('Y Acceleration [G]')
    plt.grid()
                
    plt.subplot(313)
    plt.plot(data['dt'], data[' z (G) '])
    plt.xlabel('Time (ms)')
    plt.ylabel('Z Acceleration [G]')
    plt.grid()
    
    N = N + 1
    
    f, a = FFT(data[' x (G) '], F_S)
    
    ind = np.array(np.logical_and(f>=freq_window[0], f<=freq_window[1]))
    peak_amp = np.max(a[ind])
    peak_f = f[np.where(a==peak_amp)[0][0]]
    
    
    print '    X   : %.2f G %.2f Hz ' % (peak_amp, peak_f)
    
    
    fig = plt.figure(num=N)
    fig.subplots_adjust(hspace=0.8)
    plt.subplot(311)
    plt.title(IFILE + '\n' + data[' datetime '][0][2:-1])
    plt.plot(f, a)
    #plt.locator_params(axis='y', nbins=5)
    plt.xlabel('freq [Hz]')
    plt.ylabel('X Acceleration [G]')
    plt.grid()            
    		
    f, a = FFT(data[' y (G) '], F_S)
    
    ind = np.array(np.logical_and(f>=freq_window[0], f<=freq_window[1]))
    peak_amp = np.max(a[ind])
    peak_f = f[np.where(a==peak_amp)[0][0]]
    
    
    print '    Y   : %.2f G %.2f Hz ' % (peak_amp, peak_f)
    										
    plt.subplot(312)
    plt.plot(f, a)
    plt.xlabel('freq [Hz]')
    plt.ylabel('Y Acceleration [G]')
    plt.grid()
    
    f, a = FFT(data[' z (G) '], F_S) 

    ind = np.array(np.logical_and(f>=freq_window[0], f<=freq_window[1]))
    peak_amp = np.max(a[ind])
    peak_f = f[np.where(a==peak_amp)[0][0]]
    
    
    print '    Z   : %.2f G %.2f Hz ' % (peak_amp, peak_f)          
               
    plt.subplot(313)
    plt.plot(f, a)
    plt.xlabel('freq [Hz]')
    plt.ylabel('Z Acceleration [G]')
    plt.grid()
    
    print ' ' 

plt.show()