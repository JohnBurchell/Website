#Auto build and move sites to their respective locations

git pull origin master

#Portfolio content - This should essentially mirror the content on the git repo
#Move and push into the corect folder.

sudo cp content/* /var/www/html/johnburchell.co.uk/public_html

#Programming blog
cd ~/website/programming_blog
sudo jekyll build --destination /var/www/html/programming.johnburchell.co.uk/public_html

#Warhammer blog
cd ~/webiste/warhammer_blog
sudo jekyll build --destination /var/www/html/warhammer.johnburchell.co.uk/public_html
