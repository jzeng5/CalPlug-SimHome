import os.path
import contextlib
import wave

class PathDoesNotExistError(Exception): pass
class NotWavFileError(Exception): pass

def get_wav_path() -> str:
    '''Gets name of wav file from text and returns full path of wav'''

    path = input("Please enter the full path of the .wav file: ")
    print("Finding .wav file...")

    if os.path.exists(path):
        ext = os.path.splitext(path)[1]
        
        if ext != '.wav':
            raise NotWavFileError

        print ("File found! Opening {}...".format(os.path.basename(path)))
               
    else:
        raise PathDoesNotExistError
        
    return path


def outputToFile() -> bool:
    '''Returns true if user wants to output length of wav file into a txt file'''

    while True:
        outputToFile = input("Do you want to output the length onto a txt file? [Y/N] ")
        
        if outputToFile.upper() in ["Y", "YES"]:
            return True
        elif outputToFile.upper() in ["N", "NO"]:
            return False
        else:
            print("Invalid command! Please enter again...")

def output_length(path: str, length: float) -> None:
    '''Outputs the duration of wave file into same directory as wav file'''
    with open(path, 'w') as file:
        file.write(str(length))
            

def get_wav_length() -> None:
    '''Outputs length of wav file on the console and
       in a text file if the user desires'''
    
    try:
        path = get_wav_path()
        file_with_ext = os.path.basename(path)
        filename = file_with_ext.rstrip('.wav')
        
        with wave.open(path,'r') as file:
            frame = file.getnframes()
            rate = file.getframerate()
            duration = frame / float(rate)

            print("\n{} was {} seconds long.\n".format(file_with_ext,
                                                   duration))

            if outputToFile():
                output_path = path.rstrip(file_with_ext) + filename + '-length.txt'
                output_length(output_path, duration)
                print("Successfully written duration in file {} in path {}!\n\nClosing {}...".format(filename + 'length.txt',
                                                                                                  os.path.dirname(output_path) ,
                                                                                                  filename + 'length.txt'))
                

            print("Closing {}...".format(file_with_ext))
                
        
    except PathDoesNotExistError:
        print("Path was not valid! File not found.")
        
    except NotWavFileError:
        print("Path was not a wav file! Exiting...")

    print("Exiting wavlength.py script...")

def main() -> None:
    '''Runs the script to get duration of wav file'''
    get_wav_length()

if __name__ == '__main__':
    main()
kenmorte
