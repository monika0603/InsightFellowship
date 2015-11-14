
import random
import string
import sys
import urllib2
import os
import re
import smtplib
import socket
from urlparse import urlparse
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
s = smtplib.SMTP( "localhost", 1025 )

def sendEmail():
    
    me = "monika.pu@gmail.com"
    you = "monika.pu@gmail.com"

    msg = MIMEMultipart('alternative')
    msg['Subject'] = "Vanderbilt Tier-2 Monitoring"
    msg['From'] = me

    html = "VanderbiltTier2Monitoring.html"
    part = MIMEText(html, 'html')

    msg.attach(part)
    s = smtplib.SMTP(host='smtp.gmail.com', port=587)
    s = sendmail(me, you, msg.as_string())
    s.quit()

def page_loader(url_name_a, url_name_b, dir_name='imgs'):
    
    page_to_open1 = urllib2.urlopen(url_name_a)
    page_to_open2 = urllib2.urlopen(url_name_b)
    target_page1 = page_to_open1.read()
    target_page2 = page_to_open2.read()

    base_dir = os.path.dirname(os.path.realpath(__file__))
    dir_to_save = os.path.join(base_dir, dir_name)
    new_file_name = 'VanderbiltTier2Monitoring.html'
    if not os.path.exists(dir_to_save):
        os.makedirs(dir_to_save)

    images_on_page_a = re.findall('img .*?src="(.*?)"', target_page1)
    images_on_page_b = re.findall('img .*?src="(.*?)"', target_page2)
    internal_images_a = [img for img in images_on_page_a if img.startswith('/')]
    external_images_a = [img for img in images_on_page_a if not img.startswith('/')]

    internal_images_b = [img for img in images_on_page_b if img.startswith('/')]
    external_images_b = [img for img in images_on_page_b if not img.startswith('/')]

    for image in internal_images_a:
        image_url = '%s%s' % (page_to_open.geturl()[:-1], image)
        new_image_name = urlparse(image_url).path.split('/')[-1]
        with open(os.path.join(dir_to_save, new_image_name), 'w') as new_image:
            new_image.write(urllib2.urlopen(image_url).read())
            target_page1 = re.sub(image, new_image.name, target_page1)

    for image in internal_images_b:
        image_url = '%s%s' % (page_to_open.geturl()[:-1], image)
        new_image_name = urlparse(image_url).path.split('/')[-1]
        with open(os.path.join(dir_to_save, new_image_name), 'w') as new_image:
            new_image.write(urllib2.urlopen(image_url).read())
            target_page2 = re.sub(image, new_image.name, target_page2)

    for image_url in external_images_a:
        new_image_name = urlparse(image_url).path.split('/')[-1]
        with open(os.path.join(dir_to_save, new_image_name), 'w') as new_image:
            new_image.write(urllib2.urlopen(image_url).read())
            target_page1 = re.sub(image_url, new_image.name, target_page1)

    for image_url in external_images_b:
        new_image_name = urlparse(image_url).path.split('/')[-1]
        with open(os.path.join(dir_to_save, new_image_name), 'w') as new_image:
            new_image.write(urllib2.urlopen(image_url).read())
            target_page2 = re.sub(image_url, new_image.name, target_page2)

    with open(os.path.join(base_dir, new_file_name), 'w') as new_file:
        new_file.write(target_page1)
        new_file.write(target_page2)

if __name__ == '__main__':
    target_urla = sys.argv[1]
    target_urlb = sys.argv[2]
    if len(sys.argv) > 3:
        dir_name = sys.argv[3]
        page_loader(target_urla, target_urlb, dir_name)
    else:
        page_loader(target_url)

    sendEmail()