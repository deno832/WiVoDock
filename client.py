import socket
from ctypes import cast, POINTER
from comtypes import CLSCTX_ALL
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume
import threading
import win32api
from win32con import VK_MEDIA_PLAY_PAUSE, KEYEVENTF_EXTENDEDKEY, VK_VOLUME_UP, VK_VOLUME_DOWN, VK_MEDIA_NEXT_TRACK, VK_MEDIA_PREV_TRACK

# Ses cihazlarını bulma
devices = AudioUtilities.GetSpeakers()
interface = devices.Activate(
    IAudioEndpointVolume._iid_, CLSCTX_ALL, None)

# Ses düzeyi kontrolü için arayüzü alın
volume = cast(interface, POINTER(IAudioEndpointVolume))

# Bağlanılacak sunucunun IP adresi ve port numarası
server_ip = "192.168.1.82"
server_port = 80

# TCP soketini oluştur
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
flag = False

try:
    client_socket.connect((server_ip, server_port))
    print("Cihaza Bağlanıldı...")

    while True:
        
        received_data = client_socket.recv(4)
        data = received_data.decode().strip("\n\r")
        
        if data != "":
            if data == "1234":
                win32api.keybd_event(VK_MEDIA_PLAY_PAUSE, 0, KEYEVENTF_EXTENDEDKEY, 0)
                print("dur")
                data = "" 
                continue
            elif data == "1235":
                win32api.keybd_event(VK_MEDIA_NEXT_TRACK, 0, KEYEVENTF_EXTENDEDKEY, 0)
                print("next")
                data = "" 
                continue
            elif data == "1236":
                win32api.keybd_event(VK_MEDIA_PREV_TRACK, 0, KEYEVENTF_EXTENDEDKEY, 0)
                print("prev")
                data = "" 
                continue

            elif data == "100":
                desired_volume = 1
                print(100)
            elif data[0] == "0":
                desired_volume = 0
                print(0)
            else: 

                liste = []
                liste.append(data)
                print(data)
                
                if len(data) == 2:
                    desired_volume_str = "0." + data
                    desired_volume = float(desired_volume_str)
                elif len(data) == 1:
                    desired_volume_str = "0.0" + data
                    desired_volume = float(desired_volume_str)
                    
            volume.SetMasterVolumeLevelScalar(desired_volume, None)
        else:
            pass
            
except:
    print("Bağlantı hatası: Sunucu bulunamadı veya bağlantı reddedildi.")
finally:
    # Soketi kapat
    client_socket.close()
