#!/bin/bash
# Auto build and move sites to their respective locations

# Check command options
while [ "$1" != "" ]; do
  case $1 in
    -git | -g )
      echo "Updating from Git.."
      #Move and push into the corect folder.
      git pull origin master
      #Portfolio content - This should essentially mirror the content on the git repo
      echo "Copying static portfolio content.."
      sudo cp content/* /var/www/html/johnburchell.co.uk/public_html
  esac
  shift
done

# Always build the blog pages
# Programming blog
cd ~/website/programming_blog
echo "Entering: " $(pwd)
echo "Building Warhammer blog"
sudo jekyll build --destination /var/www/html/programming.johnburchell.co.uk/public_html
cd ~/website/warhammer_blog
echo "Entering: " $(pwd)
echo "Building Programming blog"
# Warhammer blog
sudo jekyll build --destination /var/www/html/warhammer.johnburchell.co.uk/public_html
