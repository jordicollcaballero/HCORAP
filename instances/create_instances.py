import pandas as pd
import numpy as np
from random import randrange,sample,randint,shuffle
from geopy.distance import distance
import sys
#Configuracions de (A,U,S=U*(4 o 5)): (12,30),(15,30),(20,30). 20 instancies per setting, timeout=1h
#Resultats: #resoltes (va be tenir unsats per veure), valor dels 3 components de la funcio objectiu
#Encoding estratificat per les UNSAT, opcionalment es poden fer els serveis

def main():
    # Check if arguments are provided
    if len(sys.argv) < 3:
        print("Usage: python RL_files.py <U> <A> <V> tal que S=U*V")
        return

    # Access arguments
    U = int(sys.argv[1])
    A = int(sys.argv[2])
    V = int(sys.argv[3])


    #ARGS: U, A, V tal que S=U*V
    #QUE faci un print enlloc de write a instancia.txt
    #U = 30 #Nombre d'usuaris
    #A = 12 #Nombre d'agents
    S = U*V #Nombre de serveis {}
    TS = 12*5 #Nombre de time slots
    dist = [2.5,5,15,25,35,45,60,70,80,90,95,100]
    
    gender_labels = ['M','F']
    religion_labels = ['A','B','C']
    languages_labels = ["spanish", "catalan", "english"]
    type_labels_agents = ["basic", "nurse", "cpr", "physio", "doctor"]
    type_labels_services = ["basic","basic","basic", "nurse", "cpr", "physio", "doctor"]
    race_labels = ["white", "latin-american", "black", "asian"]
    

    lat =[
    "41.3815752781548","41.3559120196836","41.3482745236520","41.4629832100058",
    "41.4205718274176","41.4200823434142","41.4410422907494","41.4131703459941",
    "41.3680119940713","41.2902486755464","41.2965206008947","41.2980521776798",
    "41.2986079051265","41.4255770243766","41.4092758094591", "41.3815752781548",
    "41.3559120196836","41.3482745236520","41.4629832100058","41.4205718274176",
    "41.4200823434142","41.4410422907494","41.4131703459941","41.3680119940713",
    "41.2902486755464","41.2965206008947","41.2980521776798","41.2986079051265",
    "41.4255770243766","41.4092758094591",'41.42571943314512', '41.392751039105555',
    '41.35116718735141', '41.33809247802784', '41.43451855484428',
    '41.42324349091003', '41.36565856781028', '41.30298814407189', '41.354503828774824',
    '41.43618584555534', '41.452198894884255', '41.42859544418204', '41.46146242527701',
    '41.454161757187', '41.29278775363571', '41.410651595095786', '41.32536911513322',
    '41.457112109822454', '41.43954030893185', '41.345301124480834']
    
    long =[
    "1.98513272094305","2.11713228334789","2.26488244348681","2.24786162179945",
    "2.15257024090813","2.07977758336782","2.13520828449359","2.25664519810334",
    "2.16001633375526","2.21418980601852","2.09379793713258","2.20410100139758",
    "2.14110807067940","2.14907409962812","2.25351119355848","1.98513272094305",
    "2.11713228334789","2.26488244348681","2.24786162179945","2.15257024090813",
    "2.07977758336782","2.13520828449359","2.25664519810334","2.16001633375526",
    "2.21418980601852","2.09379793713258","2.20410100139758","2.14110807067940",
    "2.14907409962812","2.25351119355848" ,'2.100611806694838',
    '2.0807996590720337', '2.132849141471388', '1.985288086166848', '2.2223406568719897',
    '2.229928800274779', '2.059975472030265','2.213569669047887', '2.2125448615948855',
    '2.0933763181707583', '2.23744921400335','2.1977106640908475', '2.0788496607528915',
    '2.1117169221059244', '1.9954369335694504','2.1769278975224493', '2.0649224533187933',
    '2.169528377762095', '2.116419981764856','2.003883727259133']
    
    
    def create_random_labels(labels, min_labels, max_labels, num):
        rows = []
        for i in range(num):
            rows.append([x for x in sample(labels, randrange(min_labels,max_labels+1))])
        return rows
    
    # Agents 
    df_agents = pd.DataFrame()
    
    df_agents['Id'] = [x for x in range(A)]
    df_agents['Age'] = [randint(25, 60) for x in range(A)]
    df_agents['Qualifications'] = create_random_labels(type_labels_agents, 1, 3, A)
    df_agents['Location'] = [";".join(x) for x in zip(*[sample(lat,A), sample(long,A)])]
    df_agents['Region'] = np.random.choice(religion_labels, A)
    df_agents['Gender'] = np.random.choice(gender_labels, A)
    df_agents['Languages'] = create_random_labels(languages_labels, 1, 2, A)
    df_agents['Race'] = np.random.choice(race_labels, A)
    
    TSA = []
    for a in range(A):
        hours=randint(4, 12)
        if randint(0,1):
            TSA.append(([1]*hours+[0]*(12-hours))*5)
        else:
            TSA.append(([0]*(12-hours)+[1]*hours)*5)
    
    df_agents['TSA'] = TSA
    
    #Usuaris
    
    df_usuaris = pd.DataFrame()
    
    df_usuaris['Id'] = [x for x in range(U)]
    df_usuaris['Age'] = [randint(70, 90) for x in range(U)]
    df_usuaris['Location'] = [";".join(x) for x in zip(*[sample(lat,U), sample(long,U)])]
    #df_usuaris['Region'] = np.random.choice(religion_labels, U)
    df_usuaris['Gender'] = np.random.choice(gender_labels, U)
    df_usuaris['Languages'] = np.random.choice(race_labels, U)#create_random_labels(languages_labels, 1, 1, U)
    df_usuaris['Race'] = np.random.choice(race_labels, U)
    
    #Serveis
    def serviceTimeSlot(dist=dist):
        repetitions = 1
        duration = randint(2, 5)
        slot = randint(0,99)
        for i in range(len(dist)):
            if slot<dist[i]:
                day = [0]*i+[1]*duration+[0]*(len(dist)-duration-i)
                if len(day)>12:
                    day = day[-12:]
                break
        service = [day]*repetitions+[[0]*12]*(5-repetitions)
        shuffle(service)
        return np.array(service)
    
    def serviceCreation():
        df_serveis = pd.DataFrame()
    
        df_serveis['Id'] = [x for x in range(S)]
        df_serveis['user'] = [sample(list(df_usuaris['Id']),1)[0] for x in range(S)]
        df_serveis['Type'] = np.random.choice(type_labels_services, S)#create_random_labels(type_labels, 0, 1, S)
        df_serveis['TimeSlot'] = [serviceTimeSlot() for x in range(S)]
        return df_serveis
    
    df_serveis = None
    
    while df_serveis is None:
        df_serveis = serviceCreation()
        t = df_serveis[['user','TimeSlot']].groupby('user').sum().reset_index()
        valid = True
        for i in range(t.shape[0]):
            if np.sum(t['TimeSlot'][i] > 2) > 0:
                valid = False
        if valid == False:
            df_serveis = None
    
    SU = df_serveis[['user','Id']].groupby(by=['user']).agg(list)['Id'].to_list()
    
    SEQ = df_serveis[['user','Type','Id']].groupby(by=['user','Type']).agg(list)['Id'].to_list()
    
    def sim_score(agent,user,service,non_valid=-1000):
            sim = 0
            if service.Type in agent.Qualifications :
                sim -= user.Age - agent.Age
                if agent.Gender == user.Gender:
                    sim += 50
                num_common_lang = len(set(user.Languages) & set(agent.Languages))
                if num_common_lang == 1:
                    sim += 100
                if agent.Race == user.Race:
                    sim += 25
                sim -= int(distance(agent.Location, user.Location).km)
            else:
                 sim = non_valid
            return sim
    
    def compute_quantiles(r,q,non_valid=-1000):
        for (x,y), value in np.ndenumerate(r):
            pass
            if value>=q[2]:
                r[x,y]=4
            elif value>=q[1]:
                r[x,y]=3
            elif value>=q[0]:
                r[x,y]=2
            elif value > -1000:
                r[x,y]=1
            else:
                r[x,y]=0
        return r
    
    r = np.ones((A,S))
    for a in df_agents.iterrows():
        a=a[1]
        for s in df_serveis.iterrows():
            s=s[1]
            r[a.Id,s.Id] = sim_score(a,df_usuaris.iloc[s.user],s)
    
    r = compute_quantiles(r,np.quantile(r.flatten()[r.flatten()>-1000],[0.25,0.50,0.75]))
    np.unique(r, return_counts=True)
    
    
    #Print
    
    print('#U')
    print(U)
    print('#S')
    print(S)
    print('#A')
    print(A)
    print('#TS')
    print(TS)
    print('#SU')
    for e in SU:
        print(str(e).replace('[','').replace(']','').replace(',',''))
    print('#SEQ')
    for e in SEQ:
        print(str(e).replace('[','').replace(']','').replace(',',''))
    print('#TSA(i)')
    for e in df_agents['TSA'].to_list():
        print(str(e).replace('[','').replace(']','').replace(',',''))
    print('#TSS(i)')
    for e in df_serveis['TimeSlot'].to_list():
        print(str(e).replace('[','').replace(']','').replace(',',''))
    print('#r(i,j)')
    for e in r.tolist():
        print(str(e).replace('[','').replace(']','').replace(',','').replace('.0',''))
    print('#P')
    print('-1')
    print('#HN(i)')
    for e in df_agents['TSA'].to_list():
        if sum(e)<=35:
            print(sum(e))
        else:
            print('35')
    print('#HE(i)')
    for e in df_agents['TSA'].to_list():
        if sum(e)<=35:
            print('0')
        else:
            print(sum(e)-35)
        
if __name__ == "__main__":
    main()

