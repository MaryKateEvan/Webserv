from django.db import models

class Image(models.Model):
    image = models.ImageField(upload_to='usrimg/')
    uploaded_at = models.DateTimeField(auto_now_add=True)
