import csv 
import cv2
import numpy as np

from keras.models import Sequential
from keras.layers import Flatten, Dense, Conv2D, MaxPooling2D
from keras.layers import Cropping2D, Dropout, Activation, Lambda
from sklearn.model_selection import train_test_split
import sklearn

CSV_FILE = './forward/driving_log.csv'
CSV_FILE1 = './data_reverse_lap/driving_log.csv'
CSV_FILE2 = './data_recover2/driving_log.csv'
CSV_FILE3 = './data_recover/driving_log.csv'
<<<<<<< HEAD
CSV_FILE4 = './new_data/recover/driving_log.csv'
=======
CSV_FILE4 = './recover/driving_log.csv'
>>>>>>> 33b7dd55ebb3ccbdfbf3e6634e4040c2708890a7
CSV_FILE5 = './data_lap1/driving_log.csv'
IMG_FOLDER = './new_data/forward/IMG/'
IMG_FOLDER1 = './data_reverse_lap/IMG/'
IMG_FOLDER2 = './data_recover2/IMG/'
IMG_FOLDER3 = './data_recover/IMG/'
IMG_FOLDER4 = './new_data/recorver/IMG/'
IMG_FOLDER4 = './data_lap1/IMG/'

def augment_imgs(images, measurements):
        
    augmented_images, augmented_measurements = [], []
    for image, measurement in zip(images, measurements):
        augmented_images.append(image)
        augmented_measurements.append(measurement)
        augmented_images.append(cv2.flip(image,1))
        augmented_measurements.append(measurement*-1.0)

    return augmented_images, augmented_measurements


lines = []
def read_files():
    for i in [CSV_FILE, CSV_FILE1, CSV_FILE2,CSV_FILE3, CSV_FILE4, CSV_FILE5]:
        with open(i) as csvfile:
            reader = csv.reader(csvfile)
            for line in reader:
                lines.append(line)


def generator(samples, batch_size=64, center_only=True):
    num_samples = len(samples)
    while 1: # Loop forever so the generator never terminates
        sklearn.utils.shuffle(samples)
        for offset in range(0, num_samples, batch_size):
            batch_samples = samples[offset:offset + batch_size]
            images = []
            measurements = []
            for batch_sample in batch_samples:
                source_path=batch_sample[0]
                filename=source_path.split('/')
                source_path = './'+filename[-3]+'/'+filename[-2]+'/'+filename[-1]
                image = cv2.imread(source_path)
                images.append(image)
                measurement = float(batch_sample[3])
                measurements.append(measurement)

                image = cv2.flip(image,1)
                images.append(image)
                measurement = measurement*-1.0
                measurements.append(measurement)

                if center_only:
                	continue

                source_path = batch_sample[1]
                image = cv2.imread(source_path)
                images.append(image)
                measurement = float(batch_sample[3])+0.15
                measurements.append(measurement)

                source_path = batch_sample[2]
                image = cv2.imread(source_path)
                images.append(image)
                measurement = float(batch_sample[3])-0.15
                measurements.append(measurement)

            # trim image to only see section with road
            X_train = np.array(images)
            y_train = np.array(measurements)
            yield sklearn.utils.shuffle(X_train, y_train)



def main():
    read_files()

    train_samples, validation_samples = train_test_split(lines, test_size=0.2)
    print(len(train_samples))
    print(len(validation_samples))
    train_generator = generator(train_samples, batch_size=64, center_only=True)
    validation_generator = generator(validation_samples, batch_size=64, center_only=True)

    

    model = Sequential()
    model.add(Lambda(lambda x: x/255.0 - 0.5, input_shape=(160,320,3)))

    model.add(Cropping2D(cropping=((70,25), (0,0))))

<<<<<<< HEAD
    model.add(Conv2D(24,5,5,subsample=(2,2),activation="relu"))
    model.add(Dropout(0.5))
    model.add(Conv2D(36,5,5,subsample=(2,2),activation="relu"))
    model.add(Dropout(0.5))
    model.add(Conv2D(48,5,5,subsample=(2,2),activation="relu"))
    model.add(Dropout(0.5))
    model.add(Conv2D(64,3,3,subsample=(1,1),activation="relu"))
=======
    model.add(Conv2D(24,(5,5),strides=(2,2),activation="relu"))
    model.add(Dropout(0.5))
    model.add(Conv2D(36,(5,5),strides=(2,2),activation="relu"))
    model.add(Dropout(0.5))
    model.add(Conv2D(48,(5,5),strides=(2,2),activation="relu"))
    model.add(Dropout(0.5))
    model.add(Conv2D(64,(3,3),strides=(1,1),activation="relu"))
>>>>>>> 33b7dd55ebb3ccbdfbf3e6634e4040c2708890a7
#    model.add(Dropout(0.5))
    model.add(Conv2D(64,(3,3),strides=(1,1),activation="relu")) 
#    model.add(Dropout(0.5))

    model.add(Flatten())
    model.add(Dense(100))
 #   model.add(Dropout(0.5))
    model.add(Dense(50))
 #   model.add(Dropout(0.5))
    model.add(Dense(10))
 #   model.add(Dropout(0.5))
    model.add(Dense(1))

    model.compile(loss='mse', optimizer='adam')
<<<<<<< HEAD
    model.fit_generator(train_generator,samples_per_epoch=len(train_samples)*2, \
            validation_data=validation_generator,  nb_val_samples=len(validation_samples)*2, nb_epoch=10)

    model.save('test1.h5')
=======
    model.fit_generator(train_generator,steps_per_epoch=len(train_samples)/64, \
            validation_data=validation_generator,  validation_steps=len(validation_samples)/64, nb_epoch=5)

    model.save('test2.h5')
>>>>>>> 33b7dd55ebb3ccbdfbf3e6634e4040c2708890a7
    print("model saved")


if __name__ == "__main__":
    main()
