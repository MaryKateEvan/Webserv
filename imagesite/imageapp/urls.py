from django.urls import path
from . import views

urlpatterns = [
    path('', views.image_gallery, name='image_gallery'),
    path('upload/', views.upload_image, name='upload_image'),
    path('delete/<int:image_id>/', views.delete_image, name='delete_image'),
]
