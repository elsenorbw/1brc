import vaex as vx

filename = 'measurements.txt'
filename = '100k.txt'

#df = vx.from_ascii(filename, seperator=";", names=["location", "tmp"])
df = vx.open(filename, seperator=";", names=["location", "tmp"])

the_group = df.groupby(df.location, agg=vaex.agg.mean(df.tmp))

print(the_group)

