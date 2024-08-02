import pandas as pd
import matplotlib.pyplot as plt

# CSV dosyasını okuyun
df = pd.read_csv('record.csv')

# Verileri işleme: Epoch zamanlarını milisaniye cinsinden hesaplayın
df['StartEpoch'] = (df['StartEpoch'] - df['StartEpoch'].min()) / 1e6  # Mikro saniyeden milisaniyeye dönüştürme
df['EndEpoch'] = (df['EndEpoch'] - df['EndEpoch'].min()) / 1e6  # Mikro saniyeden milisaniyeye dönüştürme

# Grafik çizme
plt.figure(figsize=(10, 6))

# Her bir öncelik için başlangıç ve bitiş zamanları arasındaki çizgiyi çizme
for index, row in df.iterrows():
    plt.plot([row['Priority'], row['Priority']], [row['StartEpoch'], row['EndEpoch']], 'o-', label=f'Priority {row["Priority"]}')

plt.xlabel('Priority')
plt.ylabel('Epoch Time (ms)')
plt.title('Thread Execution Times by Priority')
plt.legend()
plt.grid(True)

# Grafiği göster
plt.show()
