import ipaddress


# Définir  tous les sous-réseaux de ton plan VLSM


subnets = [
    "172.16.9.128/28", "172.16.9.0/25", "172.16.0.0/22", "172.16.4.0/22", "172.16.8.0/24",
    "172.16.68.96/28", "172.16.68.64/27", "172.16.64.0/23", "172.16.66.0/23", "172.16.68.0/26",
    "172.16.134.128/27", "172.16.134.64/26", "172.16.132.0/23", "172.16.128.0/22", "172.16.134.0/26",
    "172.16.145.96/29", "172.16.145.0/26", "172.16.144.0/25", "172.16.144.128/25", "172.16.145.64/27",
    "192.168.1.0/30", "192.168.1.4/30", "192.168.1.8/30", "192.168.1.12/30", "192.168.1.16/30",
    "192.168.1.20/30", "192.168.1.24/30", "192.168.1.28/30", "192.168.1.32/30", "192.168.1.36/30",
    "192.168.1.40/30"
]


# Conversion en objets ip_network


nets = [ipaddress.ip_network(n) for n in subnets]



# Vérification des chevauchements



overlaps = []
for i in range(len(nets)):
    for j in range(i + 1, len(nets)):
        if nets[i].overlaps(nets[j]):
            overlaps.append((subnets[i], subnets[j]))



# Résultat


if overlaps:
    print("Chevauchements détectés :")
else:
    print("Aucun chevauchement détecté")