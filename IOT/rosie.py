import speech_recognition as sr

hotword = 'rose'

def monitora_microfone():
    microfone = sr.Recognizer()
    with sr.Microphone() as source:
        while True:
            print("Aguardando comando:")
            audio = microfone.listen(source)

            try:
                trigger = microfone.recognize_google(audio, language = 'pt-BR')
                trigger = trigger.lower()

                if hotword in trigger:
                    print('Comando: ', trigger)
                    break
                    
            except sr.UnknownValueError:
                print("Google Speech Recognition could not understand audio")
            except sr.RequestError as e:
                print("Could not request results from Google Speech Recognition service; {0}".format(e)) #google speech recognition



monitora_microfone()