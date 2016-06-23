# -*- coding: utf-8 -*-
"""
Created on Thu Jun 23 18:31:51 2016

@author: wlamont
"""

#import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

IDIR = '/media/wlamont/4AAC-26A3'

IFILE = 'LOGGER41.CSV'

data    = pd.read_csv(IDIR + '/' +  IFILE, sep='\t', header=0, skiprows=[1])
data['dt'] = data['millis '][:] - data['millis '][0] 

fig = plt.figure(num=1)
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

plt.show()