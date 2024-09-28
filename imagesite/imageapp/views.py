from django.shortcuts import render, redirect, get_object_or_404
from .models import Image
from .forms import ImageUploadForm

def image_gallery(request):
    images = Image.objects.all()
    return render(request, 'gallery.html', {'images': images})

def upload_image(request):
    if request.method == 'POST':
        form = ImageUploadForm(request.POST, request.FILES)
        if form.is_valid():
            form.save()
            return redirect('image_gallery')
    else:
        form = ImageUploadForm()
    return render(request, 'upload.html', {'form': form})

def delete_image(request, image_id):
    image = get_object_or_404(Image, id=image_id)
    if request.method == 'POST':
        image.delete()
        return redirect('image_gallery')
    return render(request, 'delete_confirmation.html', {'image': image})
