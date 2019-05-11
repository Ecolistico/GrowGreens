# Importar librerías
import pandas as pd
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
import datetime as dt
import os
import shutil
import copy
import six
import warnings

# Librerías para creación de PDF
import time
import locale
from reportlab.lib.enums import TA_JUSTIFY, TA_CENTER
from reportlab.lib.pagesizes import letter
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, Image
from reportlab.lib.styles import getSampleStyleSheet, ParagraphStyle
from reportlab.lib.units import inch

def render_mpl_table(data, col_width=3.0, row_height=0.625, font_size=14,
                     header_color='#40466e', row_colors=['#f1f1f2', 'w'], edge_color='w',
                     bbox=[0, 0, 1, 1], header_columns=0,
                     ax=None, **kwargs):
    if ax is None:
        size = (np.array(data.shape[::-1]) + np.array([0, 1])) * np.array([col_width, row_height])
        fig, ax = plt.subplots(figsize=size)
        ax.axis('off')

    mpl_table = ax.table(cellText=data.values, bbox=bbox, colLabels=data.columns, **kwargs)

    mpl_table.auto_set_font_size(False)
    mpl_table.set_fontsize(font_size)

    for k, cell in  six.iteritems(mpl_table._cells):
        cell.set_edgecolor(edge_color)
        if k[0] == 0 or k[1] < header_columns:
            cell.set_text_props(weight='bold', color='w')
            cell.set_facecolor(header_color)
        else:
            cell.set_facecolor(row_colors[k[0]%len(row_colors) ])
    return ax


warnings.filterwarnings("ignore")

filenames=(dt.datetime.now()-dt.timedelta(days=1)).strftime("%Y-%m-%d")

file='Data/{}.csv'.format(filenames)
#file='Data/2018-09-08.csv'
file_reduced='Data_reduced/Data_reduced.csv'

# Leer historial histórico de datos
data_reduced=pd.read_csv('Data_reduced/Data_reduced.csv', encoding = "ISO-8859-1") # Leer CSV reducido
data_reduced = data_reduced.drop(['Unnamed: 0'],1) # Eliminar columna índice

# Pre-filtro de información
data = pd.read_csv(file) # Leer CSV

data=data.dropna() # Quitar valores na/nan
data=data.drop_duplicates() # Filtra filas repetidas
data=data.reset_index(drop=True) # Reasigna índices

if (len(data!=0)): # Si hay datos entonces...
    # Recuperar datos de la fecha a partir del nombre
    year=[]
    month=[]
    day=[]
    count=-1
    for i in range(len(file)):
        if (file[i]=='/'):
            count=0
            continue
        if ((file[i]=='-' or file[i]=='.')):
            count+=1
        if (count==0 and (file[i]!='-' and file[i]!='.') ):
            year.append(file[i])
        if (count==1 and (file[i]!='-' and file[i]!='.') ):
            month.append(file[i])
        if (count==2 and (file[i]!='-' and file[i]!='.') ):
            day.append(file[i])

    ano=0
    mes=0
    dia=0
    for i in range(len(year)):
        ano+=int(year[i])*10**(len(year)-1-i)
    for i in range(len(month)):
        mes+=int(month[i])*10**(len(month)-1-i)
    for i in range(len(day)):
        dia+=int(day[i])*10**(len(day)-1-i)    

    mydate = dt.date(ano,mes,dia)
    # Recuperar datos de la fecha a partir del nombre

    # Filtrar encabezados repetidos
    hora = data[data.columns[0]]
    for i in range(len(data)):
        if (str(hora[i])==hora.name):
            data = data.drop(i)
    data=data.reset_index(drop=True)

    # Asignar formato para tiempo igual a: HHMMSS permitira comprar fácilmente y hacer promedios para reducir los datos
    minuto = data[data.columns[1]]
    segundo = data[data.columns[2]]


    # Reordenar tablas

    # Agregar columna tiempo
    data['Tiempo']=0
    for i in range(len(data)):
        data['Tiempo'][i] = (dt.time(int(data.Hora[i]),int(data.Minuto[i]),int(data.Segundo[i]))) # Crear nueva columna con formato HHMMSS
    data = data.drop(['Hora','Minuto','Segundo'],1) # Eliminar columnas (Hora, Minuto, Segundo)

    # Reordenar columnas
    columnas = data.columns.tolist() 
    columnas = columnas[-1:] + columnas[:-1]
    data = data[columnas] 

    # Agregar columna fecha
    data['Fecha'] = mydate
    # Reordenar columnas
    columnas = data.columns.tolist() 
    columnas = columnas[-1:] + columnas[:-1]
    data = data[columnas] 

    nombre_columnas=list(data)

    # Convertir datos string to int or float
    for i in range(2,len(nombre_columnas)):
        data[nombre_columnas[i]] = data[nombre_columnas[i]].astype(float)


    # Crear copia de la información para aplicar filtros, reducirla y filtrar valores inconsistentes de los sensores
    data1=(data.loc[0:287,:]).copy()
    data1.loc[:,nombre_columnas[2]:]=0

    # Ciclo para reducir el ruido de los sensor, así como el número de datos
    dt_hora=0
    dt_minuto=0
    dt_segundo=0

    missed_data_index=[]

    for i in range(288):
        data1.loc[i] =data[ (data["Tiempo"]<dt.time(dt_hora,dt_minuto+4,dt_segundo+59)) & (data["Tiempo"]>=dt.time(dt_hora,dt_minuto,dt_segundo))].sum()
        try:
            data1.loc[i, nombre_columnas[2]:]/=data[ (data["Tiempo"]<dt.time(dt_hora,dt_minuto+4,dt_segundo+59)) & (data["Tiempo"]>=dt.time(dt_hora,dt_minuto,dt_segundo))].count()
        except:
            missed_data_index.append(i)
        data1.loc[i,"Tiempo"]=dt.time(dt_hora,dt_minuto,dt_segundo)
        data1.loc[i,"Fecha"]=mydate
        dt_minuto+=5
        if dt_minuto==60:
            dt_minuto=0
            dt_hora+=1

    # Ciclo para rellenar espacios vacíos (Lapsos de tiempo sin registro de mediciones)
    latest1_data=0
    latest2_data=0

    if len(missed_data_index)!=0: 
        aux = missed_data_index[len(missed_data_index)-1]
    else:
        aux=-3

    aux1 = 0

    # Si faltan las últimas mediciones del día rellenar con la última medición disponible
    if (aux == len(data1)-1):
        for i in range(len(missed_data_index)-1,0,-1):
            if( missed_data_index[i]-missed_data_index[i-1] != 1 ):
                data1.loc[ aux ,nombre_columnas[2]:] = data1.loc[missed_data_index[i]-1,nombre_columnas[2]:]
                aux=-2
                break

    # Si faltan las últimas mediciones del día rellenar con la última medición disponible
    if len(missed_data_index)!=0: 
        if (aux!=-2 and missed_data_index[0]!=0):
            data1.loc[ aux ,nombre_columnas[2]:] = data1.loc[missed_data_index[0]-1,nombre_columnas[2]:]

    # Interpolar el resto de datos faltantes
    for i in range(len(data1)-1):
        if (i in missed_data_index and i!=0):
            latest1_data=data1.loc[i-1, nombre_columnas[2]:]
            aux1=i
            while (i in missed_data_index):
                i+=1
            if(i<len(data1)):
                latest2_data=data1.loc[i, nombre_columnas[2]:]
            else:
                latest2_data=data1.loc[len(data1)-1, nombre_columnas[2]:]
            for j in range(aux1,i):
                data1.loc[j,nombre_columnas[2]:]= (latest1_data+latest2_data)/2


    plt.close()
    plt.clf()
    ## Obtener imágenes con los datos

    # Ajuste polinomio temperatura interior
    F_temp1 = np.polyfit(data1.index.values , data1[nombre_columnas[2]], 90)
    BestFit_temp1 = np.polyval(F_temp1, data1.index.values)
    #plt.plot(data1["Tiempo"], BestFit_temp1,"r-")
    plt.plot(data1["Tiempo"], data1[nombre_columnas[2]],"r-")
    red_patch = mpatches.Patch(color='red', label='Temperatura Interior')

    # Ajuste polinomio temperatura LED
    F_temp2 = np.polyfit(data1.index.values , data1[nombre_columnas[3]], 90)
    BestFit_temp2 = np.polyval(F_temp2, data1.index.values)
    #plt.plot(data1["Tiempo"], BestFit_temp2,"b-")
    plt.plot(data1["Tiempo"], data1[nombre_columnas[3]],"b-")
    blue_patch = mpatches.Patch(color='blue', label='Temperatura LEDs')
    
    # Ajuste polinomio temperatura Exterior
    F_temp3 = np.polyfit(data1.index.values , data1[nombre_columnas[4]], 90)
    BestFit_temp3 = np.polyval(F_temp3, data1.index.values)
    #plt.plot(data1["Tiempo"], BestFit_temp3,"g-")
    plt.plot(data1["Tiempo"], data1[nombre_columnas[4]],"g-")
    green_patch = mpatches.Patch(color='green', label='Temperatura Exterior')
    
    # Ajuste polinomio temperatura COzir
    F_temp4 = np.polyfit(data1.index.values , data1[nombre_columnas[6]], 90)
    BestFit_temp4 = np.polyval(F_temp4, data1.index.values)
    #plt.plot(data1["Tiempo"], BestFit_temp3,"g-")
    plt.plot(data1["Tiempo"], data1[nombre_columnas[6]],"orange")
    orange_patch = mpatches.Patch(color='orange', label='Temperatura COZIR')  

    plt.ylabel('Temperatura [°C]',rotation=0,labelpad=30)
    plt.xlabel('Hora')
    plt.axhline(22, color='k',linestyle='--')
    plt.axhline(15, color='k', linestyle='--')
    black_patch = mpatches.Patch(color='k', label='Rango recomendado')
    plt.legend(fontsize=10,handles=[red_patch,blue_patch,green_patch,orange_patch,black_patch],loc='best')
    plt.xlim(0,86400)
    plt.ylim(0,35)
    plt.locator_params(axis='both', nbins=5)
    plt.title('Temperatura',loc='center')
    plt.savefig('temp.png',bbox_inches = 'tight')
    #plt.show()
    plt.clf()


    # Ajuste polinomio humedad interior
    F_hum1 = np.polyfit(data1.index.values , data1[nombre_columnas[5]], 90)
    BestFit_hum1 = np.polyval(F_hum1, data1.index.values)
    #plt.plot(data1["Tiempo"], BestFit_hum1,"r-")
    plt.plot(data1["Tiempo"], data1[nombre_columnas[5]],"r-")
    red_patch = mpatches.Patch(color='red', label='Humedad Interior')

    # Ajuste polinomio humedad COzir
    F_hum2 = np.polyfit(data1.index.values , data1[nombre_columnas[7]], 90)
    BestFit_hum2 = np.polyval(F_hum2, data1.index.values)
    #plt.plot(data1["Tiempo"], BestFit_hum2,"b-")
    plt.plot(data1["Tiempo"], data1[nombre_columnas[7]],"b-")
    blue_patch = mpatches.Patch(color='blue', label='Humedad COZIR')

    # Ajuste polinomio humedad 3
    #F_hum3 = np.polyfit(data1.index.values , data1[nombre_columnas[7]], 90)
    #BestFit_hum3 = np.polyval(F_hum3, data1.index.values)
    #plt.plot(data1["Tiempo"], BestFit_hum3,"g-")
    #green_patch = mpatches.Patch(color='green', label='Humedad Exterior')
    
    plt.ylabel('Humedad (%)',rotation=0,labelpad=20)
    plt.xlabel('Hora')
    plt.axhline(60, color='k',linestyle='--')
    plt.axhline(80, color='k',linestyle='--')
    black_patch = mpatches.Patch(color='k', label='Rango recomendado')
    plt.legend(fontsize=10,handles=[red_patch,blue_patch,black_patch],loc='best')
    plt.xlim(0,86400)
    plt.ylim(0,100)
    plt.locator_params(axis='both', nbins=5)
    plt.title('Humedad relativa',loc='center')
    plt.savefig('hum.png',bbox_inches = 'tight')
    plt.clf()

    # Déficit de presión de vapor
    DVP = (0.61078*np.exp((17.27*data1[nombre_columnas[2]])/(237.3+data1[nombre_columnas[2]])))*(1-data1[nombre_columnas[5]]/100)
    plt.plot(data1["Tiempo"], DVP,"b-")
    blue_patch = mpatches.Patch(color='blue', label='DVP medido')
    
    plt.ylabel('KPa',rotation=0,labelpad=20)
    plt.xlabel('Hora')
    plt.axhline(0.77, color='orange',linestyle='--')
    plt.axhline(0.46, color='cyan',linestyle='--')
    orange_patch = mpatches.Patch(color='orange', label='Control día')
    cyan_patch = mpatches.Patch(color='cyan', label='Control noche')
    plt.legend(fontsize=10,handles=[blue_patch,orange_patch,cyan_patch],loc='best')
    plt.xlim(0,86400)
    plt.ylim(0,2)
    plt.locator_params(axis='both', nbins=5)
    plt.title('Déficit de Presión de Vapor',loc='center')
    plt.savefig('DVP.png',bbox_inches = 'tight')
    plt.clf()

    # Datos volumen solución inyectada
    #plt.plot(data1["Tiempo"], 20-data1[nombre_columnas[10]]-data1[nombre_columnas[10]][1],"g-")
    #green_patch = mpatches.Patch(color='green', label='Solución iny.')
    
    # Datos volumen agua utilizada
    #plt.plot(data1["Tiempo"], (data1[nombre_columnas[13]]),"b-")
    #blue_patch = mpatches.Patch(color='blue', label='Agua utilizada')
    
    fig, ax1 = plt.subplots()
    
    # Datos volumen solución inyectada
    ax1.plot(data1["Tiempo"], data1[nombre_columnas[10]][1]-data1[nombre_columnas[10]],"r-")
    ax1.set_xlabel('Hora')
    ax1.set_xlim(0,86400)
    # Make the y-axis label, ticks and tick labels match the line color.
    ax1.set_ylabel('Sol. iny. [Lts]', color='r')
    ax1.tick_params('y', colors='r')
    ax1.set_ylim(0,5)

    # Datos volumen agua utilizada
    ax2 = ax1.twinx()
    ax2.plot(data1["Tiempo"], (data1[nombre_columnas[13]]),"b-")
    ax2.set_ylabel('H20 Utilizada [Lts]', color='b')
    ax2.tick_params('y', colors='b')
    ax2.set_ylim(0,35)

    fig.tight_layout()
    plt.savefig('vol.png',bbox_inches = 'tight')
    plt.clf()
    
    #plt.ylabel('Litros',rotation=0)
    #plt.xlabel('Hora')
    #plt.legend(fontsize=10,handles=[blue_patch,green_patch],loc='best')
    ##plt.legend(fontsize=10,handles=[blue_patch,red_patch,green_patch],loc='best')
    #plt.xlim(0,86400)
    #plt.ylim(0,30)#.max()))
    #plt.locator_params(axis='both', nbins=5)
    #plt.title('Volumen acumulado (agua/nutriente)',loc='center')
    #plt.savefig('vol.png',bbox_inches = 'tight')
    #plt.show()
    #plt.clf()


    # Datos Presión
    plt.plot(data1["Tiempo"], data1[nombre_columnas[9]],"g-")
    green_patch = mpatches.Patch(color='green', label='Presión (agua)')

    plt.ylabel('Presión [Psi]',rotation=0,labelpad=20)
    plt.xlabel('Hora')
    plt.axhline(100, color='k',linestyle='--')
    plt.axhline(80, color='k',linestyle='--')
    black_patch = mpatches.Patch(color='k', label='Rango recomendado')
    plt.legend(fontsize=10,handles=[green_patch,black_patch],loc='best')
    plt.xlim(0,86400)
    plt.ylim(0,125)
    plt.locator_params(axis='both', nbins=5)
    plt.title('Presión en línea de agua',loc='center')
    plt.savefig('pres.png',bbox_inches = 'tight')
    #plt.show()
    plt.clf()

    # Datos CO2 MQ-135
    plt.plot(data1["Tiempo"], data1[nombre_columnas[12]],"g-")
    green_patch = mpatches.Patch(color='green', label='CO2 (ppm) - MQ135')

    # Datos CO2 COzir
    plt.plot(data1["Tiempo"], data1[nombre_columnas[8]],"b-")
    blue_patch = mpatches.Patch(color='blue', label='CO2 (ppm) - COZIR')

    plt.ylabel('CO2 [ppm]',rotation=0,labelpad=20)
    plt.xlabel('Hora')
    plt.axhline(406, color='k',linestyle='--')
    black_patch = mpatches.Patch(color='k', label='Promedio mundial')
    plt.legend(fontsize=10,handles=[blue_patch,green_patch,black_patch],loc='best')
    plt.xlim(0,86400)
    plt.ylim(200,1000)
    plt.locator_params(axis='both', nbins=5)
    plt.title('Calidad del aire',loc='center')
    plt.savefig('CO2.png',bbox_inches = 'tight')
    #plt.show()
    plt.clf()

    table1=copy.copy(data.describe())
    table2=copy.copy(data1.describe())

    initial_letter = nombre_columnas[2][0] + nombre_columnas[2][1]
    count=0

    for i in range(2,len(nombre_columnas)):
        if( (nombre_columnas[i][0]+nombre_columnas[i][1]) == initial_letter):
            count+=1
        else:
            initial_letter = nombre_columnas[i][0] + nombre_columnas[i][1]
            count=1
        while ('{}{}'.format(initial_letter,str(count)) in list(table1)):
            count+=1
        
        table1.rename(columns={nombre_columnas[i]: '{}{}'.format(initial_letter,str(count))}, inplace=True)
        table2.rename(columns={nombre_columnas[i]: '{}{}'.format(initial_letter,str(count))}, inplace=True)
        data1.rename(columns={nombre_columnas[i]: '{}{}'.format(initial_letter,str(count))}, inplace=True)
        
    list2=['Count','Mean', 'Std Dev', 'Min', '25%','50%','75%', 'Max']
    table1['Stadistic']=list2
    table2['Stadistic']=list2

    # Reordenar columnas
    columnas1 = table1.columns.tolist() 
    columnas2 = table2.columns.tolist() 
    columnas1 = columnas1[-1:] + columnas1[:-1]
    columnas2 = columnas2[-1:] + columnas2[:-1]
    table1 = table1[columnas1] 
    table2 = table2[columnas2] 


    fig_table1=render_mpl_table(table1.round(2), header_columns=1, col_width=1.25)
    fig_table2=render_mpl_table(table2.round(2), header_columns=1, col_width=1.25)

    fig1 = fig_table1.get_figure()
    fig1.savefig("tabla1.png")
    fig2 = fig_table2.get_figure()
    fig2.savefig("tabla2.png")

    plt.clf()
    plt.close('all')

    ## Crear reporte en PDF

    # Definir idioma en español para el formato de la fecha
    locale.setlocale(locale.LC_ALL,"")

    f_name="Reporte_{}.pdf".format(mydate)

    doc = SimpleDocTemplate(f_name,pagesize=letter,
                            rightMargin=36,leftMargin=36,
                            topMargin=36,bottomMargin=18)
    Story=[]

    logo = "Image/Sippys-logo.png"
    fig_tabla1='tabla1.png'
    fig_tabla2='tabla2.png'
    figura='temp.png'
    figura1='hum.png'
    figura2='DVP.png'
    figura3='vol.png'
    figura4='pres.png'
    figura5='CO2.png'

    formatted_time = time.strftime("%A, %d de %B del %Y a las %H:%M:%S")
    full_name = "Sistemas Integrados Para la Producción y Síntesis"
    address_parts = ["Head Quarters", "Villa Victoria, Estado de México"]

    im = Image(logo, 5*inch, 3*inch)
    Story.append(im)

    styles=getSampleStyleSheet()
    styles.add(ParagraphStyle(name='Justify', alignment=TA_JUSTIFY))
    styles.add(ParagraphStyle(name='Center', alignment=TA_CENTER))
    Story.append(Spacer(1, 12))


    # Create return address
    ptext = '<font size=13><b>%s</b></font>' % full_name
    Story.append(Paragraph(ptext, styles["Normal"]))
    Story.append(Spacer(1, 12))
    for part in address_parts:
        ptext = '<font size=11>%s</font>' % part.strip()
        Story.append(Paragraph(ptext, styles["Normal"]))   

    Story.append(Spacer(1, 24))

    ptext = '<font size=12><b>Fecha en que se generó el reporte: </b></font><font size=12>%s</font>' % formatted_time 
    Story.append(Paragraph(ptext, styles["Normal"]))
    Story.append(Spacer(1, 12))

    ptext = '<font size=12><b>Sistema: </b>{}</font>'.format('Wall-G2')
    Story.append(Paragraph(ptext, styles["Justify"]))
    Story.append(Spacer(1, 12))

    ptext = '<font size=12><b>Fecha en que se obtuvieron los datos: </b>{}</font>'.format(mydate)
    Story.append(Paragraph(ptext, styles["Justify"]))
    Story.append(Spacer(1, 12))

    ptext = '<font size=11>Este reporte fue elaborado partiendo de un total de {0} mediciones,     las cuales fueron filtradas y reducidas para terminar con un total de {1} datos.     Este reporte incluye las estadísticas de la muestra original y la muestra reducida para permitir que el usuario     verifique por si mismo si la reducción es válida. </font>'.format(int((data[nombre_columnas[2]].describe())[0]),int((data1[list(data1)[2]].describe())[0]))
    Story.append(Paragraph(ptext, styles["Justify"]))
    Story.append(Spacer(1, 310))

    ptext = '<font size=11><b> Estadística de las {0} mediciones originales</b></font>'.format(int((data[nombre_columnas[2]].describe())[0]))
    Story.append(Paragraph(ptext, styles["Center"]))
    Story.append(Spacer(1, 2))

    im = Image(fig_tabla1, 10*inch, 4*inch)
    Story.append(im)
    Story.append(Spacer(1, 2))

    ptext = '<font size=11><b> Estadística de los {0} datos reducidos </b></font>'.format(int((data1[list(data1)[2]].describe())[0]))
    Story.append(Paragraph(ptext, styles["Center"]))
    Story.append(Spacer(1, 2))

    im = Image(fig_tabla2, 10*inch, 4*inch)
    Story.append(im)
    Story.append(Spacer(1, 110))

    ptext = '<font size=11><b> Se adjuntan las gráficas para un mejor análisis visual. </b></font>'
    Story.append(Paragraph(ptext, styles["Center"]))
    Story.append(Spacer(1, 12))

    im = Image(figura, 5*inch, 3*inch)
    Story.append(im)
    Story.append(Spacer(1, 12))

    im = Image(figura1, 5*inch, 3*inch)
    Story.append(im)
    Story.append(Spacer(1, 12))

    im = Image(figura2, 5*inch, 3*inch)
    Story.append(im)
    Story.append(Spacer(1, 12))

    im = Image(figura3, 5*inch, 3*inch)
    Story.append(im)
    Story.append(Spacer(1, 12))

    im = Image(figura4, 5*inch, 3*inch)
    Story.append(im)
    Story.append(Spacer(1, 12))
    
    im = Image(figura5, 5*inch, 3*inch)
    Story.append(im)
    Story.append(Spacer(1, 12))

    ptext = '<font size=10>Este reporte fue generado automáticamente,     si detecta información incongruente o quiere revisarla a detalle, favor de escribir al siguiente     correo electrónico: <i><b>jmcasimar@sippys.com.mx</b></i> describiendo su caso. </font>'
    Story.append(Paragraph(ptext, styles["Justify"]))
    Story.append(Spacer(1, 12))

    im=0

    doc.build(Story)

    (data_reduced.append(data1)).round(2).to_csv(file_reduced)

    # Borrar imagenes creadas para generar reporte
    try:
        os.remove(fig_tabla1)
        os.remove(fig_tabla2)
        os.remove(figura)
        os.remove(figura1)
        os.remove(figura2)
        os.remove(figura3)
        os.remove(figura4)
        os.remove(figura5)
    except:
        print('Archivos siendo utilizados no pudieron ser eliminados')

    # Cambiar documento a carpeta de Reportes
    shutil.copyfile(f_name, 'Reports/{}'.format(f_name))
    os.remove(f_name)