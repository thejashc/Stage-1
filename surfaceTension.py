import numpy as np

import matplotlib as mpl
mpl.use('pdf')
import matplotlib.pyplot as plt

# In[7]:


data = np.loadtxt('./data/pTens.dat')

data = data
rows = data.shape[0]
print(data.shape)


# In[8]:


#pxx = data[:,9]  + data[:,18] + data[:,27] + data[:,36]
#pyy = data[:,13] + data[:,22] + data[:,31] + data[:,40]
#pzz = data[:,17] + data[:,26] + data[:,35] + data[:,44]
Lx = 15.
Ly = 15.
Lz = 150.
volume = Lx * Ly * Lz

startLine = 5

data = data[startLine:,:]
pxx = data[:,9]  + data[:,18]  #+ data[:,27] + data[:,36]
pyy = data[:,13] + data[:,22]  #+ data[:,31] + data[:,40]
pzz = data[:,17] + data[:,26]  #+ data[:,35] + data[:,44]

# In[9]:

plt.figure(1)
plt.plot(pxx, 'r-')
plt.plot(pyy, 'g-')
plt.plot(pzz, 'b-')

pxxAvg = np.mean( pxx )
pyyAvg = np.mean( pyy )
pzzAvg = np.mean( pzz )

surfTension = ( Lz / 4 ) *( 2*pzzAvg - ( pxxAvg + pyyAvg ) )

print( surfTension )

plt.savefig('./plots/pTens.pdf')

#plt.plot(pColxx,'k')
#plt.plot(pColyy,'m')
#plt.plot(pColzz,'c')


# In[10]:
avgWindow = 25
avgStartIdx = 0
avgEndIdx = int(rows/avgWindow)*avgWindow

noOfElements = int( ( avgEndIdx - avgStartIdx ) / avgWindow ) + 1

surfaceTension = np.zeros( (noOfElements,1) )

Lperp = Lz
N = 9216

for i in np.arange( 0, noOfElements, 1 ):

    idxStart = i
    idxEnd = i + avgWindow
    pxxAvgTemp = np.mean(pxx[idxStart:idxEnd])
    pyyAvgTemp = np.mean(pyy[idxStart:idxEnd])
    pzzAvgTemp = np.mean(pzz[idxStart:idxEnd])

    surfaceTension[i] = (Lperp/4.0) * ( 2*pzzAvgTemp - (pxxAvgTemp + pyyAvgTemp ))
    temperature = np.mean(data[:,18])*(volume/N)

print(temperature)
plt.figure(2)
plt.plot( surfaceTension )

plt.xlabel(r'$t$')
plt.ylabel(r'$\gamma_{lv}$')

plt.savefig('./plots/surfaceTension.pdf')
