
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
from email.MIMEImage import MIMEImage

def sendEmail():
    
    strFrom = 'monika.pu@gmail.com'
    strTo = 'monika.pu@gmail.com'

    msgRoot = MIMEMultipart('related')
    msgRoot['Subject'] = 'Vanderbilt Tier-2 Monitoring'
    msgRoot['From'] = strFrom
    msgRoot['To'] = strTo
    msgRoot.preamble = 'This is a multi-part message in MIME format.'

    msgAlternative = MIMEMultipart('alternative')
    msgRoot.attach(msgAlternative)

    # We reference the image in the IMG SRC attribute by the ID we give it below
    msgText = MIMEText('This is the alternative plain text message.')
    msgAlternative.attach(msgText)

    # Send the email (this example assumes SMTP authentication is required)

    smtp = smtplib.SMTP('smtp.gmail.com',587)
    smtp.ehlo()
    smtp.starttls()
    smtp.connect('smtp.gmail.com')
    smtp.login(USERNAME, PASSWORD)
    smtp.sendmail(strFrom, strTo, msgRoot.as_string())
    smtp.quit()

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
    #new_file_name = '%s.html' % ''.join(random.choice(string.ascii_uppercase + string.ascii_lowercase) for _ in range(10))
    
    name = {}
    name['url_name'] = 'target_1', 'target_2', 'target_3'
    lineNumber = 0
    for line in fileHandle:
        link = line.rstrip()
        print link
        lineNumber += 1
        print lineNumber
        if(lineNumber == 1):
            target_page1 = page_loader(link)
        if(lineNumber == 2):
            target_page2 = page_loader(link)

    with open(os.path.join(new_file_name), 'w') as new_file:
        new_file.write(target_page1)
        new_file.write(target_page2)


