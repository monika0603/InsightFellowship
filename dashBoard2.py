import random
import string
import sys
import urllib2
import os
import re
import socket
from urlparse import urlparse

def page_loader(url_name_a):
    
    page_to_open1 = urllib2.urlopen(url_name_a)
    target_page1 = page_to_open1.read()

    images_on_page_a = re.findall('img .*?src="(.*?)"', target_page1)
    internal_images_a = [img for img in images_on_page_a if img.startswith('/')]
    external_images_a = [img for img in images_on_page_a if not img.startswith('/')]

    for image in internal_images_a:
        image_url = '%s%s' % (page_to_open.geturl()[:-1], image)
        new_image_name = urlparse(image_url).path.split('/')[-1]
        with open(os.path.join(new_image_name), 'w') as new_image:
            new_image.write(urllib2.urlopen(image_url).read())
            target_page1 = re.sub(image, new_image.name, target_page1)

    for image_url in external_images_a:
        new_image_name = urlparse(image_url).path.split('/')[-1]
        with open(os.path.join(new_image_name), 'w') as new_image:
            new_image.write(urllib2.urlopen(image_url).read())
            target_page1 = re.sub(image_url, new_image.name, target_page1)

    return target_page1

if __name__ == '__main__':
    target_urla = sys.argv[1]
    fileHandle = open(target_urla)
    new_file_name = 'VanderbiltTier2Monitoring.html'
    list = ['a', 'b', 'c', 'd', 'e']

    lineNumber = 0
    for line in fileHandle:
        link = line.rstrip()
        print link
        lineNumber = lineNumber + 1
        list[lineNumber] = page_loader(link)
    
        
    with open(os.path.join(new_file_name), 'w') as new_file:
        new_file.write(list[1])
        new_file.write(list[2])
        new_file.write(list[3])
        new_file.write(list[4])




